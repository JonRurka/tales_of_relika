import subprocess
import os
import base64
import zlib
import json
from termcolor import colored # install via 'pip install termcolor'
from pathlib import Path

MAX_PACK_SIZE = 10 * 1024 * 1024

parent_dir = Path(__file__).parent.parent.resolve()

final_data_path = os.path.join(parent_dir, 'build/src/Debug/data').replace('\\', '/')
output_extension = 'pack'

if not os.path.exists(final_data_path):
    os.makedirs(final_data_path)

shader_resources = [];
shader_offset = 0;
shader_resource_file_bytes = b""

data_resources = []
data_offset = 0
data_resource_bytes = {}
data_current_pack = 1

global_use_spirv = True;
do_compress = True;


def reset():
    global data_resources
    global data_offset
    global data_resource_bytes
    global data_current_pack
    data_resources.clear()
    data_offset = 0;
    data_resource_bytes.clear()
    data_current_pack = 1

# Returns {success (bool), is_spirv (bool), content bytes}
def compile_shader(working_dir, resource_path, file_path, file_name):
    global global_use_spirv
    use_spirv = False
    spirv_opengl = False
    spirv_vulkan = False
    
    content = ""
    spirv_target = ""
    with open(file_path, 'r') as file_obj:
        #asset_bytes = zlib.compress(file_obj.read(), level=9)
        content = file_obj.read()
    if ("SPIRV_OPENGL" in content):
        spirv_opengl = True
        spirv_target = 'opengl4.5'
    if ("SPIRV_VULKAN" in content):
        spirv_vulkan = True
        spirv_target = 'vulkan1.0'
    use_spirv = (spirv_opengl or spirv_vulkan)
    
    if use_spirv and global_use_spirv:
        result = subprocess.run(
            ['glslc.exe', file_name, '-o', '-', f'--target-env={spirv_target}', '-O0', '-g'],
            #['glslangValidator.exe', '-G', '-o' 'tmp.spv']
            capture_output=True,  
            cwd=working_dir
        )
        if result.returncode == 0:
            output_bytes = result.stdout
            #compressed_data = zlib.compress(output_bytes, level=9)
            decompressed_data = output_bytes
            return dict(success=True, is_spirv=True, content=decompressed_data);
        else:
            print('###### COMPILATION ERROR ########')
            print(f"shader '{resource_path}' complication failed with return code: {result.returncode}")
            #print(f"Error: {result.stderr}")
            output_str = str(result.stderr)
            #if 'error generated.' in output_str:
            output_lines = output_str.split('\\n')
            print(f'Errors Generated for {file_name}:');
            for x in range(len(output_lines)):
                print (f"\t{output_lines[x].replace('\\r', '').replace('\\n', '').replace("\\'", "'")}")
            print('#################################')
            return dict(success=False, is_spirv=True, content=b'');
    else:
        asset_bytes = b""
        with open(file_path, 'rb') as file_obj:
            #asset_bytes = zlib.compress(file_obj.read(), level=9)
            asset_bytes = file_obj.read()
        return dict(success=True, is_spirv=False, content=asset_bytes)
    """
    result = subprocess.run(
        #['glslc.exe', file, '-o', '', '--target-env=opengl4.5'],
        ['glslangValidator.exe', '-G', '-o' 'tmp.spv']
        capture_output=True,  
        cwd=working_dir
    )
    if result.returncode == 0:
        print(f"shader '{resource_path}' complication failed with return code: {result.returncode}")
        output_str = str(result.stderr)
        if 'ERROR:' in output_str:
            output_lines = output_str.split('\\n')
            print(f'Errors Generated for {file}:');
            for x in range(len(output_lines)):
                print (output_lines[x].replace('\\r', '').replace('\\n', ''))
        return "";
    """

def process_shader_asset(file_path, rel_path):
    directory = Path(file_path).parent
    file_name = Path(file_path).name
    resource_path = rel_path.replace('/', '::');
    shader_compile_res = compile_shader(directory, resource_path, file_path, file_name)
    shader_bytes = shader_compile_res['content']
    shader_size = len(shader_bytes)
    shader_compressed_bytes = zlib.compress(shader_bytes, level=9)
    shader_compressed_size = len(shader_compressed_bytes)
    #print(len(shader_bytes))
    
    #print(f"Adding shader {resource_path}. {shader_size} bytes decompressed. {shader_compressed_size} bytes compressed.")
    
    if shader_bytes == b"" or not shader_compile_res['success']:
        return
        
    print(f"Shader '{resource_path}' (SPIRV: {shader_compile_res['is_spirv']}) compiled successfully.")
    
    global shader_resources;
    global shader_offset;
    global shader_resource_file_bytes;
    
    shader_resources.append(dict(
        resource_name=resource_path,
        offset=shader_offset,
        #size=shader_size,
        size=shader_compressed_size,
        pack_file = 1,
        use_spirv=shader_compile_res['is_spirv'],
    ));
    #shader_offset = shader_offset + shader_size
    shader_offset = shader_offset + shader_compressed_size
    
    #shader_resource_file_bytes = shader_resource_file_bytes + shader_bytes;
    shader_resource_file_bytes = shader_resource_file_bytes + shader_compressed_bytes;

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
    #if data_current_pack not in data_resources:
    #    data_resources[data_current_pack] = []
        
    data_resources.append (dict(
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
    map_str = json.dumps(resource_map);
    map_str = map_str.encode("utf-8")
    base64_bytes = base64.b64encode(map_str)
    base64_string = base64_bytes.decode("ascii")
    file_path = f"{final_data_path}/s000.dat"
    print(f'Creating resource pack data "{file_path}"')
    with open(file_path, "wb+") as file:
        file.write(base64_bytes)
        
    file_data = data
    file_name = f"s001.{output_extension}"
    file_path = f"{final_data_path}/{file_name}"
    print(f'Creating resource pack "{file_path}"')
    with open(file_path, "wb+") as file:
        file.write(file_data)
    
    
def serialize_resources(prefix_letter, resource_map, data_map):
    global final_data_path
    global output_extension
    
    map_str = json.dumps(resource_map);
    map_str = map_str.encode("utf-8")
    base64_bytes = base64.b64encode(map_str)
    base64_string = base64_bytes.decode("ascii")
    file_path = f"{final_data_path}/{prefix_letter}000.dat"
    print(f'Creating resource pack data "{file_path}"')
    with open(file_path, "wb+") as file:
        file.write(base64_bytes)
    
    for key, value in data_map.items():
        file_data = value
        file_name = f"{prefix_letter}{key:03d}.{output_extension}"
        file_path = f"{final_data_path}/{file_name}"
        print(f'Creating resource pack "{file_path}"')
        with open(file_path, "wb+") as file:
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