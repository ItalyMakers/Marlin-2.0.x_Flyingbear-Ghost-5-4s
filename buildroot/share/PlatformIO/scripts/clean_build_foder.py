Import("env")
import os
import shutil
import time
import configparser

def getList():
    list= ["assets", "MksWifi.bin"]
    
    parser= configparser.ConfigParser()
    parser.read('platformio.ini')
    envs= parser['platformio']['default_envs']

    iniFolderFiles= os.listdir('ini')
    for iniFile in iniFolderFiles:
        file= os.path.join('ini', iniFile)
        parser.read(file)
        envName = 'env:' + envs
        if envName in parser:
            if 'board_build.firmware' in parser[envName]:
                list.append(parser[envName]['board_build.firmware'])
                return list
	
    list.append('Robin_nano35.bin')
    return list

def removeFiles(source, target, env):
	time.sleep(0.5)
	print("*** Cleaning Folder *** ")
	preserved_files = getList()
 	# ["assets", "MksWifi.bin", "Robin_nano35.bin"]

	build_path = os.path.join(env.Dictionary("PROJECT_BUILD_DIR"), env.Dictionary("PIOENV"))
	files = os.listdir(build_path)

	for el in files:
		fullpath = os.path.join(build_path, el)
		if os.path.exists(fullpath) and not(el in preserved_files):
			if os.path.isdir(fullpath):
				shutil.rmtree(fullpath)
				print(el + "	-> folder removed")
			else:
				os.remove(fullpath)
				print(el + "	-> file removed")
		else:
			print(el + " OK")


env.AddPostAction("buildprog", removeFiles)
