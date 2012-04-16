import json

package = json.load(open('package.json'))
APPNAME = package['name']
VERSION = package['version']

def set_options(opt):
  opt.tool_options('compiler_cxx')

def configure(conf):
  conf.check_tool('compiler_cxx')
  conf.check_tool('node_addon')
  conf.check_cfg(package='poker-eval', args='--cflags --libs', uselib_store='LIB_PE', mandatory=True)

def build(bld):
  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.target = 'wrapper'
  obj.source = ['src/wrapper.cc']
  obj.uselib = ['LIB_PE']
