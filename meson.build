project('the-scroll', 'cpp')
srcs = [
    'src/bytearray.cpp',
    'src/device.cpp',
    'src/fulfillment_item.cpp',
    'src/libgourou.cpp',
    'src/user.cpp',
    'src/loan_token.cpp',
    'utils/drmprocessorclientimpl.cpp',
    'lib/updfparser/src/uPDFParser.cpp',
    'lib/updfparser/src/uPDFTypes.cpp'
]
includes = [
    'include',
    'lib/updfparser/include',
    'lib/base64',
    'utils'
]
dependencies = [
    dependency('zlib', static: false),
    dependency('libcurl', static: false),
    dependency('openssl', static: false),
    dependency('libzip', static: false),
    dependency('pugixml', static: false),
]
the_scroll = static_library(
    'the-scroll',
    srcs,
    include_directories: includes,
    dependencies: dependencies,
)
executable(
    'gourou-activate', 
    'utils/adept_activate.cpp', 
    'utils/utils_common.cpp',
    link_with: the_scroll, 
    include_directories: includes, 
    dependencies: dependencies
)
executable(
    'gourou-download',
    'utils/acsmdownloader.cpp',
    'utils/utils_common.cpp',
    link_with: the_scroll,
    include_directories: includes,
    dependencies: dependencies
)
executable(
    'gourou-dedrm',
    'utils/adept_remove.cpp',
    'utils/utils_common.cpp',
    link_with: the_scroll,
    include_directories: includes,
    dependencies: dependencies
)