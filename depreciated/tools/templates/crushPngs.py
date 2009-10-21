# Find all .png files a crush them down as much as possible

import os, stat
import time
import shutil
import string
import filecmp


# Global png crush program and it's arguments
gPngCrushCmd='pngout'
gPngCrushArgs='/force'

# Class to do the crushing
class PngCrusher:

	# Initialize all class variables
	def __init__(self):
		self.componentDir='.'
		self.pngFileList=[]

	# Copy all xmsh files to create tmp files
	def findFiles(self):
		num=0

		print '\n Finding png files...'

		for root, dirs, files in os.walk(self.componentDir):
			if 'CVS' in dirs:
				dirs.remove('CVS')
			for name in files:

				# Only grab original png files
				if name[-4:]!='.png' or (len(name)>8 and name[-8]=='.out.png'):
					continue
				
				# Store list of files to create
				tmp=os.path.join(root,name)
				self.pngFileList.append(tmp)
				num=num+1
		print '',num,' png files found'

	def crushFiles(self):
		num=0

		print '\n Crushing png files...'

		# Actually go create the files
		for name in self.pngFileList:
			dst=name+'.out'
			args=gPngCrushCmd+' '+name+' '+dst+' '+gPngCrushArgs
			os.system(args)
			
			dst=dst+'.png'
			bstat=os.stat(name)
			astat=os.stat(dst)

			if astat.st_size < bstat.st_size:
				print ' File name',name,' crushed from:',bstat.st_size,' to:',astat.st_size,' bytes'
				shutil.copy(dst,name)
				num=num+1
			
			os.remove(dst)

		print '',num,' png files crushed and found smaller'
		
	def clean(self):
		num=0
		for name in self.pngFileList:
			dst=name+'.out.png'
			if os.path.exists(dst):
				os.remove(dst)
				num=num+1
		print '',num,' tmp files erased'

	
# The guts of the program 
if __name__=="__main__":
	import sys

	pc=PngCrusher();

	pc.findFiles()
	pc.clean()
	pc.crushFiles()

	print '\nComplete'
	
	sys.exit()
