project('the-scroll', 'cpp')
srcs = [
    'src/bytearray.cpp',
    'src/device.cpp',
    'src/fulfillment_item.cpp',
    'src/libgourou.cpp',
    'src/user.cpp',
    'utils/drmprocessorclientimpl.cpp',
    'lib/updfparser/src/uPDFParser.cpp',
    'lib/updfparser/src/uPDFTypes.cpp'
]
includes = [
    'include',
    'lib/updfparser/include',
    'lib/base64'
]
dependencies = [
    dependency('qt5', modules: ['Core', 'Network'], static: false),
    dependency('openssl', static: false),
    dependency('libzip', static: false),
    dependency('pugixml', static: false),
    dependency('zlib', static: false)
]
the_scroll = static_library(
    'the-scroll',
    srcs,
    include_directories: includes,
    dependencies: dependencies
)
executable(
    'gourou-activate', 
    'utils/adept_activate.cpp', 
    link_with: the_scroll, 
    include_directories: includes, 
    dependencies: dependencies
)
executable(
    'gourou-download',
    'utils/acsmdownloader.cpp',
    link_with: the_scroll,
    include_directories: includes,
    dependencies: dependencies
)
executable(
    'gourou-dedrm',
    'utils/adept_remove.cpp',
    link_with: the_scroll,
    include_directories: includes,
    dependencies: dependencies
)