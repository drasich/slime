#! /usr/bin/env python
# encoding: utf-8
import sys
#import Options

# the following two variables are used by the target "waf dist"
VERSION='0.0.1'
APPNAME='slime'

# these variables are mandatory ('/' are converted automatically)
top = '.'
out = 'build'

def options(opt):
  opt.load('compiler_c')
  opt.add_option('--exe', action='store_true', default=False,
      help='execute the program after it is built')
  opt.add_option('--debug', action='store_true', dest='debug', default=False, help='Build with DEBUG')


def configure(conf):
  #conf.env["CC"] = ["clang"]
  conf.load('compiler_c')
  #conf.check(header_name='stdio.h', features='cxx cxxprogram', mandatory=False)
  conf.check(header_name='stdio.h', features='c cprogram', mandatory=False)
  conf.check_cfg(package='eina', uselib_store='eina', atleast_version='0.0.1', args='--cflags --libs', mandatory=True)
  conf.check_cfg(package='eet', uselib_store='eet', atleast_version='0.0.1', args='--cflags --libs', mandatory=True)
  conf.check_cfg(package='evas', uselib_store='eet', atleast_version='0.0.1', args='--cflags --libs', mandatory=True)
  #conf.check_cfg(package='efl', uselib_store='efl', atleast_version='1.8.0', args='--cflags --libs', mandatory=True)
  conf.check_cfg(package='elementary', uselib_store='elementary', atleast_version='1.8.0', args='--cflags --libs', mandatory=True)
  #conf.check_cfg(package='protobuf', uselib_store='protobuf', atleast_version='0.0.0', mandatory=1, args='--cflags --libs')
  conf.check_cfg(package='libpng', uselib_store='png', atleast_version='0.0.0', mandatory=1, args='--cflags --libs')

  #conf.recurse('indefini')

  #conf.define ('DEBUG', 0)
  #conf.env['CXXFLAGS']=['-O2']
    
  # set some debug relevant config values
  #if Options.options.debug:
  conf.define ('DEBUG', 1)
  conf.env['CXXFLAGS'] = ['-O0', '-g3', '-Werror']
  conf.env.CFLAGS = ['-g', '-Werror'] 

def post(ctx):
  if ctx.options.exe:
    ctx.exec_command('./build/slime')

def pre(bld):
  print("Protoc start")
  bld.exec_command('protoc -I=proto --cpp_out=proto proto/base.proto')
  print("Protoc ended")
  bld.exec_command('./build/tool/property/property_value')


def build(bld):

  #bld.recurse('indefini')
  #bld.recurse('tool/property')

  #bld.exec_command('protoc -I=proto -I=/usr/include  --descriptor_set_out=proto/basedescriptor.proto --cpp_out=proto proto/base.proto')

  engine_lib_c_files = bld.path.ant_glob('lib/*.c')
  engine_lib_c_files += bld.path.ant_glob('lib/*/*.c')
  bld.shlib(
      source= engine_lib_c_files,
      target='engine',
      #use='elementary',
      use='eina eet evas',
      includes = 'lib',
      defines = ['EVAS_GL']
      )

  #cpp_files = bld.path.ant_glob('src/*.cpp proto/*.cc')
  #cpp_files = bld.path.ant_glob('src/*.c src/ui/*.c')
  c_files = bld.path.ant_glob('src/*.c')
  #cpp_files += bld.path.ant_glob('src/ui/*.c')
  c_files += bld.path.ant_glob('src/*/*.c')
  bld.program(
      source= c_files, 
      target='slime', 
      #use='elementary indefini protobuf',
      use='elementary png engine',
      linkflags = ['-ldl', '-rdynamic'],
      #includes = ['include'],
      #includes = 'include indefini/include proto',
      includes = 'src lib',
      defines = ['EDITOR', 'EVAS_GL']
      )

  bld.shlib(
      source= c_files, 
      target='slime', 
      #use='elementary indefini protobuf',
      use='elementary png engine',
      linkflags = ['-ldl', '-rdynamic'],
      #includes = ['include'],
      #includes = 'include indefini/include proto',
      includes = 'src lib',
      defines = ['EDITOR', 'EVAS_GL']
      )

  game_lib_c_files = bld.path.ant_glob('game/lib/*.c')
  bld.shlib(
      source= game_lib_c_files,
      target='gamelib',
      #use='myobjects',
      use='elementary',
      includes = 'lib src',
      cflags= ['-fpic']
      )

  game_cpp_files = bld.path.ant_glob('game/*.c')
  bld.program(
      source= game_cpp_files, 
      target='gameexec', 
      use='elementary png engine',
      linkflags = ['-ldl', '-rdynamic'],
      includes = 'game lib',
      defines = ['GAME', 'EVAS_GL']
      )


  #bld.objects(source='c.c', target='myobjects')

  bld.add_post_fun(post)
  #bld.add_pre_fun(pre)

