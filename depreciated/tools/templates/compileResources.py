# Resource compilation python script for mobile applications
# Uses an number of toadlet tools to construct resources for
# all mobile phones

# To use:
# Make sure all of your toadlet/tools are up to date and installed
# Copy this file to your application's build/ directory
# Change the global scale values below
# Change any other relevant settings in the __init__ function below
# Type "python compileResources.py -h" to see your options


import os, stat
import time
import shutil
import filecmp


# Global scale values
gAvatarHeadScale=0.025
gLevelScale=0.025
gSmallLevelScale=0.0005
gModelScale=0.005


# Class to build the cab and exe files from component data, exes, and dlls
# Called from main
class ResourceCompiler:

	# Initialize all class variables
	def __init__(self):
		self.componentDir='../components'
		self.originalDir=os.path.join(self.componentDir,'originals')
		self.mmshDir=os.path.join(self.componentDir,'mmsh')
		self.m3gDir=os.path.join(self.componentDir,'m3g')
		self.m3gLowmemDir=os.path.join(self.componentDir,'m3g_lowmem')
		self.pngLowmemDir=os.path.join(self.componentDir,'png_lowmem')
		self.mascotDir=os.path.join(self.componentDir,'mascotv3')
		self.levelDir=os.path.join(self.originalDir,'levels')
		self.modelDir=os.path.join(self.originalDir,'models')
		self.avatarDir=os.path.join(self.modelDir,'avatars')
		self.autoIt3=os.path.join(os.path.join(os.getenv("JTOADLET_DEPS_DIR"),'MascotCapsule'),'AutoIt3.exe')
		self.mascotCompiler=os.path.join(os.path.join(os.getenv("JTOADLET_DEPS_DIR"),'MascotCapsule'),'mascotCompiler.au3')
		self.mmshIgnoreList=['lawn_nofence.xmsh','merv_low.xmsh','elco_low.xmsh','sally_low.xmsh','rick_low.xmsh','lawn_notex.xmsh','forest_notex.xmsh','ocean_notex.xmsh','moon_notex.xmsh']
		self.m3gIgnoreList=['lawn_nofence.xmsh','merv_low.xmsh','elco_low.xmsh','sally_low.xmsh','rick_low.xmsh','lawn_notex.xmsh','forest_notex.xmsh','ocean_notex.xmsh','moon_notex.xmsh']
		self.m3gLowmemIgnoreList=['lawn.xmsh','merv_low.xmsh','elco_low.xmsh','sally_low.xmsh','rick_low.xmsh'];
		self.m3dLowmemDeleteList=['lawn_nofence_small.m3g','forest_small.m3g','ocean_small.m3g','moon_small.m3g','lawn_notex.m3g','forest_notex.m3g','ocean_notex.m3g','moon_notex.m3g']
		self.meshDontCombineList=['nucleDart.xmsh']
		self.idenIgnoreList=['lawn.xmsh','deadTree.xmsh','lawn_notex.xmsh','forest_notex.xmsh','ocean_notex.xmsh','moon_notex.xmsh']
		self.seIgnoreList=['lawn.xmsh','merv_low.xmsh','elco_low.xmsh','sally_low.xmsh','rick_low.xmsh','lawn_notex.xmsh','forest_notex.xmsh','ocean_notex.xmsh','moon_notex.xmsh']
		self.sprintIgnoreList=['lawn.xmsh','merv_low.xmsh','elco_low.xmsh','sally_low.xmsh','rick_low.xmsh','lawn_notex.xmsh','forest_notex.xmsh','ocean_notex.xmsh','moon_notex.xmsh']
		self.dontLightSubmeshList=['lawn.xmsh']
		self.tmpFileList=[]


	# Copy all xmsh files to create tmp files
	def createFiles(self):
		num=0
		
		self.tmpFileList=[]

		print '\n Create tmp xmsh files...'

		for root, dirs, files in os.walk(self.originalDir):
			if 'CVS' in dirs:
				dirs.remove('CVS')
			for name in files:

				# Only copy .xmsh files
				if name[-5:]!='.xmsh' or name[-9:]=='-tmp.xmsh':
					continue
				
				# Store list of files to create
				tmp=os.path.join(root,name)
				tmp=tmp.replace('.xmsh','-tmp.xmsh')
				self.tmpFileList.append(tmp)

		# Actually go create the files
		for name in self.tmpFileList:
			src=name.replace('-tmp.xmsh','.xmsh')
			shutil.copy(src,name)
			num=num+1

		print ' ',num,' files created'
			

	# Run the mesh optimizer on all tmp files
	def optimizeFiles(self):
		num=0
		
		print '\n Optimize files...'

		for file in self.tmpFileList:
			args='meshOptimizer '+file
			os.system(args)
			num=num+1

		print ' ',num,' files optimized'


	# Light the level files
	def lightLevelFiles(self):
		num=0
		nonum=0

		print '\n Light level files...'

		for file in self.tmpFileList:
			(head,tail)=os.path.split(file)

			if head==self.levelDir:
				f=tail.replace('-tmp.xmsh','.xmsh');
				args=''
				for nolight in self.dontLightSubmeshList:
					if f==nolight:
						args='vertexLighting -d0,0,1 -c1,1,1 -a0,0,0 -f32 -s1 '+file
						nonum=nonum+1
						break
				if args=='':
					args='vertexLighting -d0,0,1 -c1,1,1 -a0,0,0 -f32 '+file
				os.system(args)
				num=num+1

		print ' ',num,' level files lit, and ',nonum,' submeshs not lit'


	# Performs the compilation and copying of the proper file types with the proper compiler
	def compileMshFiles(self,converter,dstExt,dstDir,ignoreList):
		num=0

		for file in self.tmpFileList:
			(head,tail)=os.path.split(file)

			skip=False
			for ig in ignoreList:
				f=tail.replace('-tmp.xmsh','.xmsh')
				if f==ig:
					print ' Skipping ',f,' in ',dstExt
					skip=True
					break
			if skip==True:
				continue

			# For each level file we need two files, small and regular
			if head==self.levelDir:
				args=converter+' -s'+str(gSmallLevelScale)+' -b '+file
				os.system(args)
				src=file.replace('-tmp.xmsh','-tmp'+dstExt)
				dst=file.replace('-tmp.xmsh','_small'+dstExt)
				dst=dst.replace(self.originalDir,dstDir)
				shutil.move(src,dst)
				num=num+1

				args=converter+' -s'+str(gLevelScale)+' -b '+file
				os.system(args)
				src=file.replace('-tmp.xmsh','-tmp'+dstExt)
				dst=file.replace('-tmp.xmsh',dstExt)
				dst=dst.replace(self.originalDir,dstDir)
				shutil.move(src,dst)
				num=num+1

			# Compile model files
			elif head==self.modelDir:
				args=converter+' -s'+str(gModelScale)+' -b '+file
				os.system(args)
				src=file.replace('-tmp.xmsh','-tmp'+dstExt)
				dst=file.replace('-tmp.xmsh',dstExt)
				dst=dst.replace(self.originalDir,dstDir)
				shutil.move(src,dst)
				num=num+1

			# Compile avatar files, note the heads have a special scale
			elif head==self.avatarDir:
				if tail.find('Head')!=-1:
					scale=gAvatarHeadScale
				else:
					scale=gModelScale
				args=converter+' -s'+str(scale)+' -b '+file
				os.system(args)
				src=file.replace('-tmp.xmsh','-tmp'+dstExt)
				dst=file.replace('-tmp.xmsh',dstExt)
				dst=dst.replace(self.originalDir,dstDir)
				shutil.move(src,dst)
				num=num+1

		return num


	# Build the windows mobile files
	def windowsMobileFiles(self):

		print '\n Build files for windows mobile...'

		num=self.compileMshFiles('msh2mmsh','.mmsh',self.mmshDir,self.mmshIgnoreList)

		print ' ',num,' mmsh files completed'


	# Build the jsr-184 files
	def jsr184Files(self):
	
		print '\n Build regular files for jsr184...'
		
		num=self.compileMshFiles('msh2m3g','.m3g',self.m3gDir,self.m3gIgnoreList)

		print ' ',num,' m3g files completed'

		print '\n Build lowmem files for jsr184...'
		
		# The lowmem version requires the combination of textures for low-memory devices
		for file in self.tmpFileList:
			dont=""
			(head,tail)=os.path.split(file)
			for ignore in self.meshDontCombineList:
				if tail.replace('-tmp.xmsh','.xmsh')==ignore:
					print 'Dont combine textures on file:',ignore
					dont="-d "
					break
			args='meshTextureCombiner '+dont+os.path.join(self.pngLowmemDir,'textureCombine.txt')+' '+file+' '+file
			os.system(args)

		num=self.compileMshFiles('msh2m3g','.m3g',self.m3gLowmemDir,self.m3gLowmemIgnoreList)
		
		# The lowmem version also deletes unneeded level-small models after compiling
		for file in self.m3dLowmemDeleteList:
			dst=os.path.join(self.m3gLowmemDir,'levels');
			dst=os.path.join(dst,file);
			print 'Removing file:',dst
			os.remove(dst);
		
		print ' ',num,' lowmem m3g files completed'


	# Build the mascotv3 files - a messy enough process that it requires it's own function
	def mascotv3Files(self):
		num=0
		traFileList=[]

		print '\n Build files for mascotv3...'

		# Mascot capsule requires the combination of textures for low-memory devices
		for file in self.tmpFileList:
			dont=""
			(head,tail)=os.path.split(file)
			for ignore in self.meshDontCombineList:
				if tail.replace('-tmp.xmsh','.xmsh')==ignore:
					print 'Dont combine textures on file:',ignore
					dont="-d "
					break
			args='meshTextureCombiner '+dont+os.path.join(self.mascotDir,'textureCombine.txt')+' '+file+' '+file
			os.system(args)

		# Compile all .xmsh files to .bac, but leave results in the original dir
		for file in self.tmpFileList:
			args='msh2bac -f '+file
			os.system(args)

		# We now need a list of .tra files for the model animations
		for root, dirs, files in os.walk(self.originalDir):
			if 'CVS' in dirs:
				dirs.remove('CVS')
			for name in files:
				tmp=os.path.join(root,name)
				if name[-4:]=='.tra':
					traFileList.append(tmp)
		
		# Now generate the mbac files with the mascotCompiler 
		# using the AutoIt3 tool, and the corresponding .bac and .tra files
		for file in self.tmpFileList:
			trasForBac=[]
			bac=file.replace('-tmp.xmsh','-tmp.bac')

			# We need all of the corresponding .tra files with a name prefix
			# equal to this .bac's
			for tra in traFileList:
				us=tra.rfind('_')
				if us!=-1 and bac.find(tra[:us])!=-1:
					trasForBac.append(tra)

			# Now rename the .bac file to remove the tmp label for the converter
			# since it automatically renames the output files
			dst=bac.replace('-tmp.bac','.bac')
			shutil.move(bac,dst)
			bac=dst
			(head,tail)=os.path.split(bac)
			
			# Now call the converter for all 3 mascotv3 compilers
			for type in ['iden','se','sprint']:
				dstDir=head.replace(self.originalDir,os.path.join(self.componentDir,type))

				# Ignore listed files
				skip=False
				if type=='iden':
					for ig in self.idenIgnoreList:
						f=tail.replace('.bac','.xmsh')
						if f==ig:
							print ' Skipping ',f,' on iden'
							skip=True
							break
				elif type=='se':
					for ig in self.seIgnoreList:
						f=tail.replace('.bac','.xmsh')
						if f==ig:
							print ' Skipping ',f,' on se'
							skip=True
							break
				elif type=='sprint':
					for ig in self.sprintIgnoreList:
						f=tail.replace('.bac','.xmsh')
						if f==ig:
							print ' Skipping ',f,' on sprint'
							skip=True
							break
				if skip==True:
					continue

				# For each level file we need two files, small and regular
				if head==self.levelDir:
					args=self.autoIt3+' '+self.mascotCompiler+' '+type+' '+dstDir+' '+str(2000*gLevelScale)+' '+bac
					for s in trasForBac:
						args=args+' '+s
					print args
					os.system(args)
					num=num+1

					sml=bac.replace('.bac','_small.bac')
					shutil.copy(bac,sml)
					args=self.autoIt3+' '+self.mascotCompiler+' '+type+' '+dstDir+' '+str(2000*gSmallLevelScale)+' '+sml
					for s in trasForBac:
						args=args+' '+s
					print args
					os.system(args)
					num=num+1

				# Compile model files
				elif head==self.modelDir:
					args=self.autoIt3+' '+self.mascotCompiler+' '+type+' '+dstDir+' '+str(2000*gModelScale)+' '+bac
					for s in trasForBac:
						args=args+' '+s
					print args
					os.system(args)
					num=num+1

				# Compile avatar files, note the heads have a special scale
				elif head==self.avatarDir:
					if tail.find('Head')!=-1:
						scale=gAvatarHeadScale
					else:
						scale=gModelScale
					args=self.autoIt3+' '+self.mascotCompiler+' '+type+' '+dstDir+' '+str(2000*scale)+' '+bac
					for s in trasForBac:
						args=args+' '+s
					print args
					os.system(args)
					num=num+1

		print ' ',num,' bac files completed'


	# Clean up 
	def clean(self):
		num=0

		print '\n Cleanup tmp and conversion files...'

		# Find and delete the tmp files
		for root, dirs, files in os.walk(self.originalDir):
			if 'CVS' in dirs:
				dirs.remove('CVS')

			for name in files:
				if name[-9:]=='-tmp.xmsh' or name[-14:]=='_combined.xmsh' or name[-5:]=='.mmsh' or name[-4:]=='.m3g' or name[-4:]=='.bac' or name[-4:]=='.tra':
					src=os.path.join(root,name)
					os.remove(src)
					num=num+1
					
		print ' ',num,' files deleted'
			

	# Clean up created resources
	def resourceClean(self):
		num=0

		print '\n Cleanup compiled resource files...'

		# Find and delete the files
		for root, dirs, files in os.walk(self.mmshDir):
			if 'CVS' in dirs:
				dirs.remove('CVS')
			for name in files:
				if name[-5:]=='.mmsh':
					src=os.path.join(root,name)
					os.remove(src)
					num=num+1

		for root, dirs, files in os.walk(self.m3gDir):
			if 'CVS' in dirs:
				dirs.remove('CVS')
			for name in files:
				if name[-4:]=='.m3g':
					src=os.path.join(root,name)
					os.remove(src)
					num=num+1
					
		for root, dirs, files in os.walk(self.m3gLowmemDir):
			if 'CVS' in dirs:
				dirs.remove('CVS')
			for name in files:
				if name[-4:]=='.m3g':
					src=os.path.join(root,name)
					os.remove(src)
					num=num+1

		for type in ['iden','se','sprint']:
			dstDir=os.path.join(self.componentDir,type)

			for root, dirs, files in os.walk(dstDir):
				if 'CVS' in dirs:
					dirs.remove('CVS')
				for name in files:
					if name[-5:]=='.mbac' or name[-5:]=='mtra':
						src=os.path.join(root,name)
						os.remove(src)
						num=num+1	
						
		print ' ',num,' files deleted'
	

	# Compile all resource files
	def compile(self):
		self.clean()

		self.createFiles()
		self.optimizeFiles()
		self.lightLevelFiles()
		self.windowsMobileFiles()
		
		self.clean()

		self.createFiles()
		self.optimizeFiles()
		self.lightLevelFiles()
		self.jsr184Files()
		
		self.clean()

		self.createFiles()
		self.optimizeFiles()
		self.lightLevelFiles()
		self.mascotv3Files()
		
		self.clean()

	
# The guts of the program 
if __name__=="__main__":
	import sys

	# Create a compiler
	rc=ResourceCompiler();

	if len(sys.argv)<2:
		# Compile all resources
		rc.compile()
		print '\nResource Compile Complete'
	else:
		# Parse arguments
		if sys.argv[1]=='-w':
			rc.clean()
			rc.createFiles()
			rc.optimizeFiles()
			rc.lightLevelFiles()
			rc.windowsMobileFiles()
			rc.clean()
			print '\nWindows Mobile Resource Compile Complete'
		elif sys.argv[1]=='-m':
			rc.clean()
			rc.createFiles()
			rc.optimizeFiles()
			rc.lightLevelFiles()
			rc.jsr184Files()
			rc.clean()
			print '\nM3G Resource Compile Complete'
		elif sys.argv[1]=='-a':
			rc.clean()
			rc.createFiles()
			rc.optimizeFiles()
			rc.lightLevelFiles()
			rc.mascotv3Files()
			rc.clean()
			print '\nMascotv3 Resource Compile Complete'
		elif sys.argv[1]=='-c':
			rc.clean()
			print '\nTmp Clean Complete'
		elif sys.argv[1]=='-r':
			rc.resourceClean()
			print '\nResource Clean Complete'
		else:
			sys.exit('Usage: resourceCompiler.py [option]\n \
\t optional choices (choose one): \n \
\t none just compiles all resources \n \
\t -h  print this message\n \
\t -w  build only windows mobile files\n \
\t -m  build only m3g files\n \
\t -a  build only mascotv3 files\n \
\t -c  clean out tmp files\n \
\t -r  clean out completed resource files\n ')

	sys.exit()
