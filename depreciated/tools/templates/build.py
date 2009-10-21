# Constructo Combat python build script
# Creates all of the needed files/cabs/exes


import os, stat
import time
import shutil
import filecmp


# Globals
gAppName='3dcc'


# Class to build the cab and exe files from component data, exes, and dlls
# Called from main
class CabBuilder:

	# Initialize all class variables
	def __init__(self,appName):
		self.appName=appName
		self.demoName=appName+'Demo'
		self.dataDir=appName+'Data/'
		self.compDir='../../components/'
		self.resourceDirs=[
			self.compDir+'base/',
			self.compDir+'mmsh/',
			self.compDir+'sounds_11khz/',
			self.compDir+'windows_mobile/',
		]
		self.displaySrcFiles=[]
		self.displayDstFiles=[]
		self.excludes=['.xmsh','.msh','.max','.cfg','.blend','.cmp','.smd','keepme.txt']
		self.dataFile='data.dat'
		self.indexFile='index.dat'
		self.rcBaseFile='JDropscape-base.rc'
	

	# Gathers all game data into a single directory 
	# Useful for creating the data/index files and
	# for seeing when files are updated
	def gatherData(self):
		rebuild=False
		if (os.path.exists(self.dataDir)==False):
			os.mkdir(self.dataDir)

		# Search through the existing data directory and look
		# for files existing there that are no-longer present
		# in the components; if so delete them
		print '\nCompare resources...'
		for root, dirs, files in os.walk(self.dataDir):
			# Multiple source directories are needed due to them being separately stored in the components directory
			srcDirList=[]
			for resourceDir in self.resourceDirs:
				srcDirList.append(os.path.join(resourceDir,root.replace(self.dataDir,'')))
			dstDir=root

			dirCmpList=[]
			for srcDir in srcDirList:
				if (os.path.exists(srcDir)==True):
					dirCmpList.append(filecmp.dircmp(srcDir,dstDir))

			# A file is only extra if it not found in ALL of the directory comparisons
			dstList=files+dirs
			for dirCmp in dirCmpList:
				bothList=(dirCmp.common)
				for item in bothList:
					if (item in dstList):
						dstList.remove(item)
			for displayFile in self.displayDstFiles:
				item=os.path.basename(displayFile)
				if (item in dstList):
					dstList.remove(item)
				
			# Delete the extra file/dir
			for dstFile in dstList:
				dstFull=os.path.join(dstDir,dstFile)
				try:
					if os.path.isfile(dstFull):
						try:
							print 'Remove old file='+dstFull
							os.remove(dstFull)
							rebuild=True;
						except OSError, e:
							print e
							rebuild=True;
					elif os.path.isdir(dstFull):
						try:
							print 'Remove old directory='+dstFull
							shutil.rmtree(dstFull,True)
							rebuild=True;
						except shutil.Error, e:
							print e
							rebuild=True;

				except Exception, e:
					print e
					rebuild=True;
					continue

		if (rebuild==False):
			print 'No old resources'

		# Copy all of the resources into a the data dir if
		# 1. They are not present in the data dir
		# 2. They need to be updated
		print '\nCopy resources...'
		for resourceDir in self.resourceDirs:
			for root, dirs, files in os.walk(resourceDir):

				if 'CVS' in dirs:
					dirs.remove('CVS')
				for name in files:
					# Skip files with extensions in excludes
					skip=False
					for ext in self.excludes:
						l=len(ext)
						if name[-l:]==ext:
							skip=True
							break
					if skip==True:
						continue

					# Compare the remaining source files timestamp 
					# againt the destination file, if present
					srcFile=os.path.join(root,name)
					dstFile=os.path.join(self.dataDir,root.replace(resourceDir,''),name)

					# See if the destination file exists
					if (os.path.isfile(dstFile)):
						srcStat=os.stat(srcFile)
						dstStat=os.stat(dstFile)

						# If we need to update, do so and flag the need to rebuild the data/index files
						if ((srcStat.st_mtime > dstStat.st_mtime) or (srcStat.st_ctime > dstStat.st_mtime)):
							print 'Update resource='+srcFile+' to destination='+dstFile
							shutil.copy(srcFile,dstFile)
							rebuild=True
					else:
						if (os.path.isdir(dstFile.replace(name,''))==False):
							print 'Make directory='+dstFile.replace(name,'')
							os.makedirs(dstFile.replace(name,''))
						print 'Copy resource='+srcFile+' to destination='+dstFile
						shutil.copy(srcFile,dstFile)
						rebuild=True

		# Do the same with the display specific data
		for i in range(0,len(self.displaySrcFiles)):
			if os.path.isfile(self.displayDstFiles[i]):
				srcStat=os.stat(self.displaySrcFiles[i])
				dstStat=os.stat(self.displayDstFiles[i])
				if ((srcStat.st_mtime > dstStat.st_mtime) or (srcStat.st_ctime > dstStat.st_mtime)):
					print 'Update resource='+self.displaySrcFiles[i]+' to destination='+self.displayDstFiles[i]
					shutil.copy(self.displaySrcFiles[i],self.displayDstFiles[i])
					rebuild=True
			else:
				print 'Copy resource='+self.displaySrcFiles[i]+' to destination='+self.displayDstFiles[i]
				shutil.copy(self.displaySrcFiles[i],self.displayDstFiles[i])
				rebuild=True

		if (rebuild==False):
			print 'No new resources'

		return rebuild


	# Recursively walks down a filetree and finds every file,
	# then generates two files as follows:
	# 1. A binary data file containing every found file one after another
	# 2. An index file containing: filename offset length
	def buildIndexData(self,dataFilesDir,demo):
		dataFile=os.path.join(dataFilesDir,self.dataFile)
		indexFile=os.path.join(dataFilesDir,self.indexFile)
		rcFile=self.rcBaseFile.replace('-base','')
		rcExt=['.bmp']
		rcList=[]
		indexList=[]
		rcBase=1000

		if (os.path.exists(dataFilesDir)==False):
			os.mkdir(dataFilesDir)

		# Open the output files
		dfile=open(dataFile,'wb')

		# For the index file
		offset=0

		# Grab files and directories
		for root, dirs, files in os.walk(self.dataDir):
			if 'CVS' in dirs:
				dirs.remove('CVS')
			for name in files:
				# Names
				fileName=os.path.join(root,name)
				indexName=fileName.replace(self.dataDir,'')
			
				# IF we have a demo version, ignore all files under any level directoy beyond 1
				if demo==True:
					if fileName.find('level02')!=-1:
						continue
					elif fileName.find('level03')!=-1:
						continue
					elif fileName.find('level04')!=-1:
						continue
					elif fileName.find('level05')!=-1:
						continue
			
				# Skip files with extensions in rcExt - instead save them for later
				skip=0
				for ext in rcExt:
					l=len(ext)
					if name[-l:]==ext:
						rcList.append(indexName)
						skip=1
						break
				if skip==1:
					continue

				# Include the rest in the dat file
				file=open(fileName,'rb')
				dfile.write(file.read())
				size=file.tell()

				# Store the name, offset, and size for inclusion into the index file
				tmp=indexName+' '+str(offset)+' '+str(size)
				indexList.append(tmp.replace('\\','/')+'\n')
				offset+=size
				file.close()
		
		# Data file done
		dfile.close()
		print '\nData file='+dataFile+' completed'
		
		# Create the new rc file
		if os.path.exists(rcFile):
			os.remove(rcFile)
		shutil.copyfile(self.rcBaseFile,rcFile)
		rfile=open(rcFile,'a')
		
		# Add the rcList files to the front of the index file and the end of the rc file
		i=0
		for rc in rcList:
			indexList.insert(i,rc.replace('\\','/')+' -1 '+str(rcBase)+'\n')
			tmp=os.path.join(self.dataDir,rc)
			rfile.write(str(rcBase)+' BITMAP \"'+tmp.replace('\\','/')+'\"\n')
			rcBase=rcBase+1
			i=i+1
		
		# Rc file finished
		rfile.close()
		print 'Resource file='+rcFile+' completed'
		
		# Create the index file
		ifile=open(indexFile,'w')
		for line in indexList:
			ifile.write(line)
		ifile.close()
		print 'Index file='+indexFile+' completed'
		

	# Just update and rebuild the data files if necessary
	def updateData(self,demo):
		# If we are building data; it is unsigned 
		if demo==True:
			dirName=self.demoName+'-unsignedFiles'
		else:
			dirName=self.appName+'-unsignedFiles'
		
		# Update all of the component data
		rebuild=self.gatherData()

		# Rebuild the index and data files, if necessary
		dataExists=os.path.exists(os.path.join(dirName,self.dataFile))
		indexExists=os.path.exists(os.path.join(dirName,self.indexFile))
		if rebuild==True:
			self.buildIndexData(dirName,demo)
		elif dataExists==False or indexExists==False:
			self.buildIndexData(dirName,demo)
			
		
	# Construct the cab and exe files
	# with optional signing
	def buildCabExe(self,signedFiles,signedCab,debug,demo,compressed):
		# Setup based on debug, and/or demo
		if debug==True:
			signedName=self.debugName
			originalName=self.debugName
			dirName=self.appName
			infFile=self.appName+'.inf'
			logFile=self.appName+'.log'
			iniFile=self.appName+'.ini'
		elif demo==True:
			signedName=self.demoName
			originalName=self.demoName
			dirName=self.demoName
			infFile=self.demoName+'.inf'
			logFile=self.demoName+'.log'
			iniFile=self.demoName+'.ini'
		else:
			signedName=self.appName
			originalName=self.appName
			dirName=self.appName
			infFile=self.appName+'.inf'
			logFile=self.appName+'.log'
			iniFile=self.appName+'.ini'

		# Add to names for signing
		if signedFiles==True:
			signedName+='-signedFiles'
			dirName+='-signedFiles'
			if(os.path.exists(dirName)==False):
				os.mkdir(dirName)
			print '\nSigned files are externally provided and must be placed in the '+dirName+' directory'
			if signedCab==True:
				signedName+='-signedCab'
			else:
				signedName+='-unsignedCab'
		else:
			signedName+='-unsignedFiles'+'-unsignedCab'
			dirName+='-unsignedFiles'

		# Check for necessary data updates
		if signedFiles==False:
			self.updateData(demo)

		# Copy the data files back down
		if os.path.exists(os.path.join(dirName,self.dataFile))==False or os.path.exists(os.path.join(dirName,self.indexFile))==False:
			print 'Data and index files must be located in: '+dirName
			return
		shutil.copyfile(os.path.join(dirName,self.dataFile),self.dataFile)
		shutil.copyfile(os.path.join(dirName,self.indexFile),self.indexFile)
		
		# Build or copy down your cab
		if signedCab==False:
			if compressed==False:
				# Use the 2003 Pocket PC tool and use the uncompressed ddf
				shutil.copyfile('cabwiz-uncompressed.ddf','cabwiz.ddf')
				# Generate the cab file
				args='cabwiz '+infFile+' /err '+logFile
				print '\nExecute command: '+args
				os.system(args)
				print '\nUncompressed Cab file complete'
			else:
				# Use the Smartphone tool and use the compressed ddf
				shutil.copyfile('cabwiz-compressed.ddf','cabwiz.ddf')
				# Generate the cab file
				args='cabwizsp '+infFile+' /err '+logFile
				print '\nExecute command: '+args
				os.system(args)
				print '\nCompressed Cab file complete'
		else:
			# Just copy down the signed cab
			if os.path.exists(os.path.join(dirName,originalName+'.cab'))==False:
				print 'Signed cab file:'+originalName+'.cab must be located in: '+dirName
				return
			shutil.copyfile(os.path.join(dirName,originalName+'.cab'),originalName+'.cab')

		# Generate the exe file
		args='ezsetup -l english -i '+iniFile+' -r readme.txt -e eula.txt -o '+originalName+'.exe.'
		print '\nExecute command: '+args
		os.system(args)
		print '\nExe file complete'
		
		# Rename the output cab and exe files
		if os.path.exists(signedName+'.cab'):
			os.remove(signedName+'.cab')
		os.rename(originalName+'.cab',signedName+'.cab')
		if os.path.exists(signedName+'.exe'):
			os.remove(signedName+'.exe ')
		os.rename(originalName+'.exe',signedName+'.exe')
		
		# Delete the copied data files
		os.remove(self.dataFile);
		os.remove(self.indexFile);


	# Build an exe with a standalone cab file
	def signCab(self,cabName,demo):
		# Setting for demo or regular
		if demo==True:
			appName=self.demoName
			originalName=self.demoName
			iniFile=self.demoName+'.ini'
		else:
			appName=self.appName
			originalName=self.appName
			iniFile=self.appName+'.ini'
			
		# Rename so the ini file picks it up
		shutil.copyfile(cabName,appName+'.cab');
		
		# Generate a signed exe file from the signed cab
		args='ezsetup -l english -i '+iniFile+' -r readme.txt -e eula.txt -o '+appName+'.exe'
		print '\nExecute command: '+args
		os.system(args)
		print '\nExe file complete'
		
		#Rename the exe file
		os.rename(appName+'.exe',cabName.replace('.cab','.exe'))


	# Delete all data/index/etc files
	def clean(self):
		print 'Cleaning out data and rc files'
	
		if(os.path.exists(self.dataDir)):
			shutil.rmtree(self.dataDir)
		if(os.path.exists(self.appName+'-unsignedFiles/')):
			shutil.rmtree(self.appName+'-unsignedFiles/')
		if(os.path.exists(self.demoName+'-unsignedFiles/')):
			shutil.rmtree(self.demoName+'-unsignedFiles/')
		rcFile=self.rcBaseFile.replace('-base','')
		if(os.path.exists(rcFile)):
			os.remove(rcFile);
		

# The guts of the program 
if __name__=="__main__":
	import sys

	# Create a builder
	builder=CabBuilder(gAppName);
	
	# Check for enough arguments
	if len(sys.argv)<2:
		sys.exit('Usage: build.py [options] [target]\n \
\t target: \n \
\t\t d - data files: collect all data into the '+builder.dataDir+' directory and rebuild the index,data,and rc files \n \
\t target options: \n \
\t\t -do - demo: build the demo files \n \
\n \
\t target: \n \
\t\t ce - cab/exe: build a cab and exe \n \
\t target options: \n \
\t\t -sf - signed files: build your cab/exe with signed files \n \
\t\t -do - demo: build a demo version \n \
\t\t -db - debug: build a debug version \n \
\t\t -uc - uncompressed: build for PocketPC2003 \n \
\n \
\t target: \n \
\t\t sc - sign cab: build an exe from a signed cab \n \
\t target options: \n \
\t\t -do - demo: build a demo exe \n \
\t REQUIRED option: \n \
\t\t (filename) - the name of the cab you wish to turn into an exe \n \
\n \
\t target: \n \
\t\t c - clean: remove all constructed files')

	# Parse arguments
	if len(sys.argv)<2:
		sys.exit('Please provide a target')
	target=sys.argv[-1]
	options=sys.argv[1:-1]
	
	if target=='d':
		demo=False
		for opt in options:
			if opt=='-do':
				demo=True
		builder.updateData(demo);
	elif target=='ce':
		demo=False
		debug=False
		signed=False
		compressed=True
		for opt in options:
			if opt=='-sf':
				signed=True
			elif opt=='-do':
				demo=True
			elif opt=='-db':
				debug=True
			elif opt=='-uc':
				compressed=False
		builder.buildCabExe(signed,False,debug,demo,compressed)
	elif target=='sc':
		demo=False
		name=[]
		if len(sys.argv)<3:
			sys.exit('requires a cab name')
			for opt in options:
				if opt=='-do':
					demo=True
				else:
					name=opt
		builder.signCab(name,demo)
	elif target=='c':
		builder.clean();

	print '\nComplete'

	sys.exit()
