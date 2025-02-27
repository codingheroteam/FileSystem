var express = require('express');
var router = express.Router();
var session = require('express-session');
const { spawn } = require('child_process');

/* GET home page. */
router.get('/', function (req, res, next) {
    res.render('login', { title: 'Express' });
});

// let i = 0;
router.get('/restart', function (req, res, next) {
    // if(i>10) {
    //     res.end();
    // }
    res.writeHead(200, {
        'Content-Type' : 'text/event-stream',
        'Access-Control-Allow-Origin':'*'
    })
    let msg = 'event: ping\n'+"data:"+JSON.stringify('123')+"" + "\n\n";
    res.write(msg);
    console.log('new')
    // return;
    var sess = req.session;
    var params = req.body;

    // 鉴权

    try {

        // let i = 0;
        // const timer = setInterval(()=>{
        //     const date = {date:new Date()}
        //     var content = 'event: ping\n'+"data:"+JSON.stringify(date)+"" + "\n\n";
        //     res.write(content);
        //     console.log(content)
        // },1000)
     
        res.connection.on("close", function(){
            res.end();
            // clearInterval(timer);
            console.log("Client closed connection. Aborting.");
            });

            

        // // 设置 SSE 响应头
        // res.setHeader('Content-Type', 'text/event-stream');
        // res.setHeader('Cache-Control', 'no-cache');
        // res.setHeader('Connection', 'keep-alive');
        // res.setHeader('Access-Control-Allow-Origin', '*');


        // 执行 Shell 脚本，并将实时输出发送给客户端
        const scriptProcess = spawn('sh', ['./routes/script/osfs_update.sh']);

        scriptProcess.stdout.on('data', data => {
            const output = data.toString();
            // 发送实时输出给前端
            console.log(output);
            let msg = 'event: load\n'+"data:"+JSON.stringify(output)+"" + "\n\n";
            res.write(msg);
        });

        scriptProcess.stderr.on('data', data => {
            const output = data.toString();
            // 发送实时错误输出给前端
            console.log(output);
            let msg = 'event: load\n'+"data:"+JSON.stringify(output)+"" + "\n\n";
            res.write(msg);
            
            // res.end();
        });

        scriptProcess.on('close', code => {
            // 脚本执行完成
            console.log('结束');
            let msg = 'event: close\n'+"data:"+"" + "\n\n";
            res.write(msg);
            // res.end();
        });
    } catch (e) {
        console.log('err', e);
    }
});


router.get('/update', function (req, res, next) {
    res.writeHead(200, {
        'Content-Type' : 'text/event-stream',
        'Access-Control-Allow-Origin':'*'
    })
    let msg = 'event: ping\n'+"data:"+JSON.stringify('123')+"" + "\n\n";
    res.write(msg);
    console.log('new')

    try {
     
        res.connection.on("close", function(){
            res.end();
            // clearInterval(timer);
            console.log("Client closed connection. Aborting.");
            });

            
        // 执行 Shell 脚本，并将实时输出发送给客户端
        const scriptProcess = spawn('sh', ['/home/Brayden/Desktop/neuIT/manage/routes/script/it_update.sh']);

        scriptProcess.stdout.on('data', data => {
            const output = data.toString();
            // 发送实时输出给前端
            console.log(output);
            let msg = 'event: load\n'+"data:"+JSON.stringify(output)+"" + "\n\n";
            res.write(msg);
        });

        scriptProcess.stderr.on('data', data => {
            const output = data.toString();
            // 发送实时错误输出给前端
            console.log(output);
            let msg = 'event: load\n'+"data:"+JSON.stringify(output)+"" + "\n\n";
            res.write(msg);
            
            // res.end();
        });

        scriptProcess.on('close', code => {
            // 脚本执行完成
            console.log('结束');
            let msg = 'event: close\n'+"data:"+"" + "\n\n";
            res.write(msg);
            // res.end();
        });
    } catch (e) {
        console.log('err', e);
    }
});

module.exports = router;
