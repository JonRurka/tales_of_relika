import subprocess
import os
import base64
import zlib
from pathlib import Path

MAX_PACK_SIZE = 10 * 1024 * 1024

parent_dir = Path(__file__).parent.parent.resolve()

final_data_path = os.path.join(parent_dir, 'build/src/Debug/data').replace('\\', '/')
output_extension = 'pack'

shader_resources = [];
shader_offset = 0;
shader_resource_file_bytes = b""

data_resources = {}
data_offset = 0
data_resource_bytes = {}
data_current_pack = 1

def reset():
    global data_resources
    global data_offset
    global data_resource_bytes
    global data_current_pack
    data_resources.clear()
    data_offset = 0;
    data_resource_bytes.clear()
    data_current_pack = 1

def compile_shader(working_dir, resource_path, file):    
    result = subprocess.run(
        ['glslc.exe', file, '-o', '-', '--target-env=opengl4.5'], 
        capture_output=True,  
        cwd=working_dir
    )

    if result.returncode == 0:
        print(f"Shader '{resource_path}' compiled successfully.")
        output_bytes = result.stdout
        compressed_data = zlib.compress(output_bytes, level=9)
        return compressed_data;
    else:
        print(f"shader '{resource_path}' complication failed with return code: {result.returncode}")
        #print(f"Error: {result.stderr}")
        output_str = str(result.stderr)
        if 'error generated.' in output_str:
            output_lines = output_str.split('\\n')
            print(f'Errors Generated for {file}:');
            for x in range(len(output_lines)):
                print (output_lines[x].replace('\\r', '').replace('\\n', ''))
        return "";

def process_shader_asset(file_path, rel_path):
    directory = Path(file_path).parent
    file_name = Path(file_path).name
    resource_path = rel_path.replace('/', '::');
    shader_bytes = compile_shader(directory, resource_path, file_name)
    shader_size = len(shader_bytes)
    #print(len(shader_bytes))
    
    if shader_bytes == "":
        return
    
    global shader_resources;
    global shader_offset;
    global shader_resource_file_bytes;
    
    shader_resources.append(dict(
        resource_name=resource_path,
        offset=shader_offset,
        size=shader_size,
        pack_file = 1
    ));
    shader_offset = shader_offset + shader_size
    
    shader_resource_file_bytes = shader_resource_file_bytes + shader_bytes;

def general_process_asset(file_path, rel_path):
    global data_resources
    global data_offset
    global data_resource_bytes
    global data_current_pack
    
    directory = Path(file_path).parent
    file_name = Path(file_path).name
    resource_path = rel_path.replace('/', '::');
    asset_bytes = b""
    with open(file_path, 'rb') as file:
        asset_bytes = zlib.compress(file.read(), level=9)
    asset_size = len(asset_bytes)
    
    if data_current_pack not in data_resource_bytes:
        data_resource_bytes[data_current_pack] = b""
    if data_current_pack not in data_resources:
        data_resources[data_current_pack] = []
        
    data_resources[data_current_pack].append (dict(
        resource_name=resource_path,
        offset=data_offset,
        size=asset_size,
        pack_file=data_current_pack
    ));
    data_offset = data_offset + asset_size
    
    data_resource_bytes[data_current_pack] = data_resource_bytes[data_current_pack] + asset_bytes;
    
    if data_offset > MAX_PACK_SIZE:
        data_current_pack = data_current_pack + 1
        data_offset = 0
 

def process_assets(directory, extension_list, func):
    # Walk through directory and its subdirectories
    global parent_dir
    directory = os.path.join(parent_dir, directory).replace('\\', '/')
    print(f"Processing files from '{directory}'")
    for root, dirs, files in os.walk(directory):        
        # Print files
        for file_name in files:
            file_path = os.path.join(root, file_name).replace('\\', '/');
            rel_path = file_path.replace(directory, '');
            if rel_path.startswith('/'):
                rel_path = rel_path[1:]
            extension = Path(rel_path).suffix;
            if (extension.lower() in extension_list):
                func(file_path, rel_path);

def serialize_shader_resources(resource_map, data):
    global final_data_path
    global output_extension
    map_str = str(resource_map);
    map_str = map_str.encode("utf-8")
    base64_bytes = base64.b64encode(map_str)
    base64_string = base64_bytes.decode("ascii")
    file_path = f"{final_data_path}/s000.dat"
    print(f'Creating resource pack data "{file_path}"')
    with open(file_path, "wb") as file:
        file.write(base64_bytes)
        
    file_data = data
    file_name = f"s001.{output_extension}"
    file_path = f"{final_data_path}/{file_name}"
    print(f'Creating resource pack "{file_path}"')
    with open(file_path, "wb") as file:
        file.write(file_data)
    
    
def serialize_resources(prefix_letter, resource_map, data_map):
    global final_data_path
    global output_extension
    
    map_str = str(resource_map);
    map_str = map_str.encode("utf-8")
    base64_bytes = base64.b64encode(map_str)
    base64_string = base64_bytes.decode("ascii")
    file_path = f"{final_data_path}/{prefix_letter}000.dat"
    print(f'Creating resource pack data "{file_path}"')
    with open(file_path, "wb") as file:
        file.write(base64_bytes)
    
    for key, value in data_map.items():
        file_data = value
        file_name = f"{prefix_letter}{key:03d}.{output_extension}"
        file_path = f"{final_data_path}/{file_name}"
        print(f'Creating resource pack "{file_path}"')
        with open(file_path, "wb") as file:
            file.write(file_data)
        

print('Processing Shaders.')
process_assets('resources/shaders', ['.comp', '.vert', '.frag'], process_shader_asset);
serialize_shader_resources(shader_resources, shader_resource_file_bytes)
#serialize_resources('s', data_resources, data_resource_bytes)
print('\n')

print('Processing Textures.')
reset()
process_assets('resources/textures', ['.png', '.jpg', '.bmp'], general_process_asset);
serialize_resources('t', data_resources, data_resource_bytes)
print('\n')

print('Processing Models.')
reset()
process_assets('resources/models', ['.obj'], general_process_asset);
serialize_resources('m', data_resources, data_resource_bytes)
print('\n')

print('Processing Data.')
reset()
process_assets('resources/data', ['.obj'], general_process_asset);
serialize_resources('d', data_resources, data_resource_bytes)