from building import *
import rtconfig

cwd     = GetCurrentDir()
src     = Glob('src/*.c')

path    = [cwd + '/']
path   += [cwd + '/src']

group = DefineGroup('ADS', src, depend = ['PKG_USING_ADS'], CPPPATH = path)
Return('group')