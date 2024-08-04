project('pl-rt', 'c',
  version: '1.04',
  license: 'MPL-2.0',
  default_options: [
    'c_std=c11',
    'b_lundef=false',
  ]
)

inc = include_directories('include')
subdir('src')

executable('plrt-test', 'plrt-test.c',
    include_directories: inc,
    link_with: libplrt)
