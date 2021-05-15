Import("env")
import os
import requests
import zipfile
import tempfile
import shutil

url = "https://github.com/makerbase-mks/Mks-Robin-Nano-Marlin2.0-Firmware/archive/master.zip"
zip_path = os.path.join(env.Dictionary("PROJECT_LIBDEPS_DIR"), "mks-assets.zip")
assets_path = os.path.join(env.Dictionary("PROJECT_BUILD_DIR"), env.Dictionary("PIOENV"), "assets")

def download_mks_assets():
	print("Downloading MKS Assets")
	r = requests.get(url, stream=True)
	# the user may have a very clean workspace,
	# so create the PROJECT_LIBDEPS_DIR directory if not exits
	if os.path.exists(env.Dictionary("PROJECT_LIBDEPS_DIR")) == False:
		os.mkdir(env.Dictionary("PROJECT_LIBDEPS_DIR"))
	with open(zip_path, 'wb') as fd:
		for chunk in r.iter_content(chunk_size=128):
			fd.write(chunk)

def copy_mks_assets():
	print("Copying MKS Assets")
	output_path = tempfile.mkdtemp()
	zip_obj = zipfile.ZipFile(zip_path, 'r')
	zip_obj.extractall(output_path)
	zip_obj.close()
	if os.path.exists(assets_path) == True and os.path.isdir(assets_path) == False:
		os.unlink(assets_path)
	if os.path.exists(assets_path) == False:
		os.mkdir(assets_path)
	base_path = ''
	for filename in os.listdir(output_path):
		base_path = filename
	for filename in os.listdir(os.path.join(output_path, base_path, 'Firmware', 'mks_font')):
		shutil.copy(os.path.join(output_path, base_path, 'Firmware', 'mks_font', filename), assets_path)
	for filename in os.listdir(os.path.join(output_path, base_path, 'Firmware', 'mks_pic')):
		shutil.copy(os.path.join(output_path, base_path, 'Firmware', 'mks_pic', filename), assets_path)
	shutil.rmtree(output_path, ignore_errors=True)

if os.path.exists(zip_path) == False:
	download_mks_assets()

if os.path.exists(assets_path) == False:
	copy_mks_assets()
 
 
custom_graphics_toolpath = os.path.join(env.Dictionary("PROJECT_DIR"), "custom_graphics") + "/"
custom_graphics_path = os.path.join(env.Dictionary("PROJECT_DIR"), "custom_graphics", "images") + "/"
custom_fonts_path = os.path.join(env.Dictionary("PROJECT_DIR"), "custom_graphics", "fonts") + "/"
assets_path = os.path.join(env.Dictionary("PROJECT_BUILD_DIR"), env.Dictionary("PIOENV"), "assets")

def convert_assets():
	print("*** Converting Assets *** ")
	for filename in os.listdir(custom_graphics_path):
		if ".png" in filename:			
			img_name=os.path.splitext(filename)[0]
			command = 'php '+custom_graphics_toolpath+'/_img_conv_core.php "name='+img_name+'&img='+custom_graphics_path+"/"+filename+'&cf=true_color&format=bin_565&out='+assets_path+'/"'
			print("-> " + filename )
			os.system(command)

def copy_fonts():
	print("*** Copying Fonts *** ")
	for filename in os.listdir(custom_fonts_path):
		if ".bin" in filename:	
			print("-> " + filename )
			shutil.copy(os.path.join(custom_fonts_path, filename), assets_path)	



# shutil.rmtree(assets_path, ignore_errors=True)
if os.path.exists(assets_path) == False:
	os.mkdir(assets_path)

convert_assets()
copy_fonts()