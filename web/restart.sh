kill  -9 $(lsof -i:4535 -t)
nohup npm run start > apprc.txt
echo 重启完成，请手动刷新