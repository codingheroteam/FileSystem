curPath=$(readlink -f "$(dirname "$0")")
echo 进入项目文件夹
cd $curPath/../../../
curPath=$(readlink -f "$(dirname "$0")")
echo 拉取最新代码
git checkout .
git pull
echo 进入文件管理系统代码文件夹
cd ./os
mkdir build
cd build
echo 编译最新代码
cmake ..
make
echo 完成

# echo 拉取最新代码
# git pull
# echo '进入web文件夹'
# cd web
# echo '下载库'
# yarn
# echo '开始构建'
# yarn run build
# cd ..

echo 重启前端
echo 进入前端文件夹
cd web