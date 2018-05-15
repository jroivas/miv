project('ahead_class_vm', 'cpp')

add_global_arguments('-std=c++14', language : 'cpp')
add_global_arguments('-fPIC', language : 'cpp')
add_global_arguments('-Wall', language : 'cpp')
add_global_arguments('-Wno-error=unused-local-typedefs', language : 'cpp')
add_global_arguments('-Wno-unused-local-typedefs', language : 'cpp')
add_global_arguments('-Wno-error=pedantic', language : 'cpp')

main_inc = include_directories('inc')
top_inc = include_directories('.')

subdir('src')
