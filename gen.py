import re

header_file = "bink.h"
func_list_file = "functions.txt"
out_file = "bink_loader_gen.c"

# Read function list
with open(func_list_file) as f:
    func_names = set(line.strip() for line in f if line.strip())

# Read header
with open(header_file) as f:
    header = f.read()

# Remove RADEXPLINK entirely
header = re.sub(r'\bRADEXPLINK\b', '', header)

functions = []

# Regex to match return type + function name + args
prototype_re = re.compile(
    r'^[ \t]*(\w[\w\s\*\d]+)[ \t]+(' + '|'.join(func_names) + r')[ \t]*\(([^)]*)\)\s*;',
    re.MULTILINE
)

for match in prototype_re.finditer(header):
    ret_type = match.group(1).strip()
    name = match.group(2).strip()
    args = match.group(3).strip()
    args = re.sub(r'\s*,\s*', ', ', args)

    # Only add * if return type is NOT already a pointer
    ptr_mark = '' if ret_type.endswith('*') else '*'

    functions.append((ret_type, name, args, ptr_mark))

# Generate C file
with open(out_file, "w") as f:
    f.write('#include "bink.h"\n#include <windows.h>\n\n')
    f.write('// Macro to define function pointer and global variable\n')
    f.write('#define DECL_FN(ret, name, args, ptr) typedef ret (__cdecl *name##_t) args; name##_t p##name = NULL;\n\n')

    # Typedefs + globals
    for ret, name, args, ptr_mark in functions:
        f.write(f'DECL_FN({ret}, {name}, ({args}), {ptr_mark})\n')

    f.write('\nHMODULE dll = NULL;\n\n')
    f.write('int LoadBinkDLL(void) {\n')
    f.write('    if (dll) return 1;\n')
    f.write('    dll = LoadLibraryA("bink2w64.dll");\n')
    f.write('    if (!dll) return 0;\n\n')

    # Assign function pointers
    for _, name, _, _ in functions:
        f.write(f'    p{name} = ({name}_t)GetProcAddress(dll, "{name}");\n')

    f.write('\n    return 1;\n}\n')
