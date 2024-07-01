project('pl-rt', 'c',
  version: '1.02.4',
  license: 'MPL-2.0',
  default_options: [
    'c_std=c99',
    'b_lundef=false',
  ]
)

inc = include_directories('include')
subdir('src')

executable('plrt-test', 'plrt-test.c',
    include_directories: inc,
    link_with: libplrt)
