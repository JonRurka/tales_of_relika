import os
from pathlib import Path

extension_list = ['.h', '.cpp', '.comp', '.vert', '.frag', '.lua', '.py'];
#exclude_dirs = ['']

lines = 0;
num_files = 0;

def count_file_lines(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
    return len(lines)
    
def count_lines(directory):
    print(f'Scanning "{directory}"...')
    global lines
    global num_files
    # Walk through directory and its subdirectories
    for root, dirs, files in os.walk(directory):        
        # Print files
        for file_name in files:
            file_path = os.path.join(root, file_name).replace('\\', '/');
            dir = Path(file_path).parent;
            extension = Path(file_name).suffix;
            if ('LuaCpp' in str(dir) or 
                'build' in str(dir) or
                'nvpro_core' in str(dir)):
                continue;
            if (extension.lower() in extension_list):
                print(f'Counting "{file_path}"...')
                num_files = num_files + 1;
                lines = lines + count_file_lines(file_path);
    
directory = os.getcwd() + '/..';

count_lines(directory + '/src/Engine')
count_lines(directory + '/src/Game')
count_lines(directory + '/src/Server')
count_lines(directory + '/src/Compute')
count_lines(directory + '/src/VoxelEngine') 
#count_lines(directory + '/src')
count_lines(directory + '/resources/shaders')
            
print(f"Found {lines} lines accross {num_files} files.")
