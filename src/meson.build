yt_subs_tracker_sources = [
  'main.c',
  'yt_subs_tracker-window.c',
  'main-backend.c',
  'parse.c',
  'conf.c',
  'readConf.c',
  'writeConf.c',
  'blank.c'
]

yt_subs_tracker_deps = [
  dependency('gio-2.0', version: '>= 2.50'),
  dependency('gtk+-3.0', version: '>= 3.22'),
  dependency('libsoup-2.4', version: '>= 2.60'),
  dependency('libxml-2.0', version: '>= 2.9')
]

gnome = import('gnome')

yt_subs_tracker_sources += gnome.compile_resources('yt_subs_tracker-resources',
  'yt_subs_tracker.gresource.xml',
  c_name: 'yt_subs_tracker'
)

executable('yt_subs_tracker', yt_subs_tracker_sources,

  dependencies: yt_subs_tracker_deps,
  install: true,
)
