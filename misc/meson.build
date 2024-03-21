project('dde-control-center-metainfo')

source_root = meson.project_source_root()

metainfo_dir = get_option ('datadir') / 'metainfo'

i18n = import('i18n')
subdir('po')

mesoninfo_i18n = i18n.itstool_join(
  input: 'org.deepin.dde.controlcenter.metainfo.xml',
  output: 'org.deepin.dde.controlcenter.metainfo.xml',
  mo_targets: i18n_result[0],
  its_files : [join_paths(meson.current_source_dir(), 'metainfo.its')]
)
