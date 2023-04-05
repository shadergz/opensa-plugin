import os,subprocess,argparse,shutil

parser = argparse.ArgumentParser()

parser.add_argument('--build', '-b', nargs='?', type=bool, const=True, default=True)
parser.add_argument('--clean', '-c', nargs='?', type=bool, const=True, default=False)

args = parser.parse_args()

rootDir = 'OpenSA'

buildDir = 'build'
ndkPath = os.getenv('NDK_PATH')
cmakeDirPath = os.getenv('ANDROID_CMAKE')

if ndkPath == '':
    print('NDK_PATH not configured')
if cmakeDirPath == '':
    print('ANDROID_CMAKE does not exist inside your system paths!')

cmakePath = os.path.join(cmakeDirPath, 'bin', 'cmake')
ninjaPath = os.path.join(cmakeDirPath, 'bin', 'ninja')

toolchain = ndkPath + '/build/cmake/android.toolchain.cmake'

buildType = ['Release', 'Debug']
debug=0

executeDir = os.path.basename(os.getcwd())

if executeDir != rootDir:
    print('Executing out of project root (%s directory), this will lead to some errors' % rootDir)

cmakeBuildCommand = cmakePath + ' -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=' +  buildType[debug] + ' -DCMAKE_TOOLCHAIN_FILE=' + toolchain +\
    ' -DANDROID_ABI=arm64-v8a -DANDROID_NDK=' + ndkPath + ' -DANDROID_PLATFORM=android-29 -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a'\
    ' -DANDROID_STL=c++_shared -DCMAKE_MAKE_PROGRAM=' + ninjaPath + ' -DCMAKE_SYSTEM_NAME=Android -DCMAKE_SYSTEM_VERSION=29 -GNinja ..'

buildDir = os.path.join(os.getcwd(), buildDir)

if args.build and not os.path.exists(buildDir):
    os.makedirs(buildDir)
    os.chdir(buildDir)
    subprocess.run(cmakeBuildCommand.split(' '))
elif args.clean and os.path.exists(buildDir):
    shutil.rmtree(buildDir)
    

