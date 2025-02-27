
let gcmd = "";       // 上一次调用的命令
let gval = "";       // 上一次调用的参数
let gcnt;
let gcmdans;    // 命令的返回结果
let gsocket;
let first = true;
let curDirs = [];

let mode = 0;


function showWarnMessage(msg) {
    ElementPlus.ElMessage({
        message: msg,
        type: 'warning',
        plain: true,
    })
}

function showSuccMessage(msg) {
    ElementPlus.ElMessage({
        message: msg,
        type: 'success',
        plain: true,
    })
}

function changeShow() {
    let term = document.querySelector("#terminalcontainer");
    let ui = document.querySelector("#uicontainer");
    // let elem = document.querySelector("#btnmode");
    if (mode == 0) {
        term.classList.remove("hide");
        ui.classList.add("hide");
        mode = 1;
        document.body.className = "green";
        // elem.innerHTML = "终端模式";
    }
    else {
        term.classList.add("hide");
        ui.classList.remove("hide");
        mode = 0;
        // elem.innerHTML = "图形模式";
        document.body.className = "blue";

    }



}

function getFileExtension(filename) {
    // 使用正则表达式匹配文件名中的后缀名
    const regex = /(?:\.([^.]+))?$/;
    const match = filename.match(regex);

    // 返回匹配的后缀名，如果没有匹配到则返回空字符串
    return match && match[1] ? match[1] : '';
}


changeShow();
changeShow();


function mergePath(path, name) {
    path = path.trim();
    name = name.trim();
    // alert(name);
    // return;
    if (path == "/") {
        return path + name;
    }
    return path + "/" + name;
}

let lastClick = -1;
let lastClickTime = 0;
let copypath = "";
function onItemClicked(diritem) {

    // let item = JSON.parse(itemjson);
    // console.log(item)
    let name = diritem.name;
    let dimode = diritem.mode;
    let descripelem = document.querySelector("#descriptitle");
    let functionlistelem = document.querySelector("#functionlist");
    let descripiconicon = document.querySelector("#descripiconicon");
    let container = document.querySelector("#container");

    container.className = "container";


    if (lastClick == diritem.id && Date.now() - lastClickTime < 500) {
        descripelem.innerHTML = "";
        functionlistelem.innerHTML = "";
        let infobox = document.querySelector('#infobox');
        infobox.innerHTML = ''
        descripiconicon.src = "";
        // 是目录
        if (dimode[0] == 'd' || diritem.lkmode == "->dir") {
            sendRequire("cd", name);
        }
        // 是文件
        else if (dimode[0] == 'f' || diritem.lkmode == "->file") {
            sendRequire("cat", name);
        }
    }
    else {
        descripelem.innerHTML = name;
        functionlistelem.innerHTML = "";


        let infobox = document.querySelector('#infobox');
        infobox.innerHTML = `
         <div>
            <div> 索引节点号：${diritem.dinode} </div>
            <div> 文件/目录大小：${diritem.disize} </div>
            <div> 所有者：${diritem.uid} </div>
            <div> 数据块：${diritem.blocks} </div>
            <div> 权限模式：${diritem.mode} </div>
            <div> 硬链接数：${diritem.dinumber} </div>
         </div>
        `;

        // 是目录
        if (dimode[0] == 'd') {
            let open = document.createElement("div");
            open.className = "mbtn";
            open.innerHTML = "进入";
            open.onclick = () => {
                sendRequire("cd", name);
            };
            functionlistelem.append(open);
            descripiconicon.src = "folder.svg";
        }

        // 是文件
        else if (dimode[0] == 'f') {
            let open = document.createElement("div");
            open.className = "mbtn";
            open.innerHTML = "打开";
            open.onclick = () => {
                sendRequire("cat", name);
            };
            functionlistelem.append(open);
            descripiconicon.src = "file.svg";
            if (getFileExtension(name) == "sh") {
                let exec = document.createElement("div");
                exec.className = "mbtn";
                exec.innerHTML = "执行";
                exec.onclick = () => {
                    sendRequire("bash", name);
                };
                functionlistelem.append(exec);
                descripiconicon.src = "bash.svg";
            }
        }

        // 是软链接
        else if (dimode[0] == 'l') {
            container.classList.add("link");

            if (diritem.lkmode == "->file") {
                let open = document.createElement("div");
                open.className = "mbtn";
                open.innerHTML = "打开";
                open.onclick = () => {
                    sendRequire("cat", name);
                };
                functionlistelem.append(open);
                descripiconicon.src = "file.svg";
            }
            else if (diritem.lkmode == "->dir") {
                let open = document.createElement("div");
                open.className = "mbtn";
                open.innerHTML = "进入";
                open.onclick = () => {
                    sendRequire("cd", name);
                };
                functionlistelem.append(open);
                descripiconicon.src = "folder.svg";
            }
            else {
                // let open = document.createElement("div");
                // open.className = "mbtn";
                // open.innerHTML = "打开";
                // open.onclick = () => {
                //     sendRequire("cat", name);
                // };
                // functionlistelem.append(open);
                descripiconicon.src = "unknown.svg";
            }
        }


        let dele = document.createElement("div");
        dele.className = "mbtn";
        dele.innerHTML = "删除";
        dele.onclick = () => {
            sendRequire("rm", "-rf " + name);
        };
        functionlistelem.append(dele);
        let copy = document.createElement("div");
        copy.className = "mbtn";
        copy.innerHTML = "复制";
        copy.onclick = () => {
            if (path == "/")
                copypath = path + name;
            else
                copypath = path + "/" + name;
            document.querySelector("#paste").disabled = false;
        };
        functionlistelem.append(copy);
        let rename = document.createElement("div");
        rename.className = "mbtn";
        rename.innerHTML = "重命名";
        rename.onclick = () => {
            var code = prompt("重命名：", name);
            if (code == null)
                return;
            sendRequire("rename", name + " " + code);
        };
        functionlistelem.append(rename);

        let mode = document.createElement("div");
        mode.className = "mbtn";
        mode.innerHTML = "修改权限";
        mode.onclick = () => {
            var code = prompt("输入权限：");
            if (code == null)
                return;
            sendRequire("chmod", code + " " + name);
        };
        functionlistelem.append(mode);

        let slk = document.createElement("div");
        slk.className = "mbtn";
        slk.innerHTML = "创建软链接";
        slk.onclick = () => {
            sendRequire("slk", mergePath(path, name) + " " + name + ".link");
        };
        functionlistelem.append(slk);

        if (dimode[0] == 'f') {
            let slk = document.createElement("div");
            slk.className = "mbtn";
            slk.innerHTML = "创建硬链接";
            slk.onclick = () => {
                var code = prompt("输入创建位置：");
                if (code == null)
                    return;
                sendRequire("hlk", mergePath(path, name) + " " + code);
            };
            functionlistelem.append(slk);
        }


    }
    lastClick = diritem.id;
    lastClickTime = Date.now();
}

let username;
let path;
function sendRequire(cmd, val = "") {
    // alert(username)

    first = false;
    gcmd = cmd;
    gval = val;
    gcnt = 0;
    gcmdans = "";
    gsocket.send(cmd + " " + val + "\n");
    curDirs = [];
}

function acceptNewData(str) {
    console.log("data", str);
    gcmdans += str;
    console.log("gcmdans is ");
    console.log(gcmdans);
    gcnt++;
    const regex1 = /.*\w+@\w+(-\w+)*:\S+\$\s*$/; // 匹配prompt



    const regex2 = /.*?(.*?)@(.*?):(.*?)\$$/;
    const match = gcmdans.match(regex2);

    if (match) {
        username = match[1];
        path = match[3];
        document.querySelector("#username").innerHTML = username;
        document.querySelector("#path").innerHTML = path;
        if (username == "root") {
            // alert(1);
        }

    } else {
        // throw new Error("Prompt string does not match the expected format.");
    }




    // 通过 prompt 判断文件尾
    // if (gcmd == "ls") {
    let datas = gcmdans.split('\n');
    let div = document.querySelector("#dircontainer");

    // 未输出完成
    if (!regex1.test(gcmdans))
        return;
    // 输出完成了


    gcmdans = "";
    // 判断是否有报错

    // 正则表达式
    const regex3 = /.*ERROR: .*/g;

    // gcmdans = "mkdir newfolder(12)\nERROR: mkdir: 索引节点已用完\n\nroot@PC:/$"



    // alert(gcmdans);


    for (let data of datas) {
        // alert(data)
        // 使用正则表达式匹配字符串
        const matches3 = data.match(regex3);
        // 输出匹配结果
        if (matches3) {
            matches3.forEach((match, index) => {
                console.log(`Match ${index + 1}: ${match}`);
                showWarnMessage(match);

            });
        } else {
            console.log("No matches found");
        }
    }


    // 如果未登录
    if (username == "traveler") {
        document.querySelector("#loginout").innerHTML = "登录";
        document.querySelector("#loginout").onclick = login;
    }
    else {
        document.querySelector("#loginout").innerHTML = "退出登录";
        document.querySelector("#loginout").onclick = logout;
    }


    if (gcmd == "ls") {
        div.innerHTML = "";
        for (let idx in datas) {
            if (idx == 0) continue;
            if (idx == 1) continue;
            if (idx == datas.length - 1) continue;

            let stritem = datas[idx];
            let vals = stritem.split('\t');

            // console.log(vals);
            curDirs.push(vals[0].trim());
            let item = document.createElement("div");
            let dimode = vals[2];
            let lkmode = vals[8];
            // alert(1)
            let diritem = { name: vals[0].trim() };


            diritem.dinode = vals[1];
            diritem.disize = vals[3];
            diritem.uid = vals[4];
            diritem.blocks = vals[5];
            diritem.id = idx;
            diritem.mode = vals[2];
            diritem.lkmode = vals[8];
            diritem.dinumber = vals[7];
            // alert(diritem)


            if (dimode[0] == 'f') {
                item.innerHTML = `
                <img class="icon" src="file.svg"/>
                <div class="itemname">${diritem.name}</div>
                `;
                if (getFileExtension(diritem.name) == "sh") {
                    item.innerHTML = `
                        <img class="icon" src="bash.svg"/>
                        <div class="itemname">${diritem.name}</div>
                    `;
                }

                item.className = "fileitem";
                item.onclick = () => { onItemClicked(diritem) };
            }
            else if (dimode[0] == 'd') {
                item.innerHTML = `
                <img class="icon" src="folder.svg"/>
                <div class="itemname">${diritem.name}</div>
            `;
                item.className = "diritem";
                item.onclick = () => { onItemClicked(diritem) };
            }
            else if (dimode[0] == 'l') {
                if (lkmode == "->file") {
                    item.innerHTML = `
                    <div class="container link">
                        <img class="icon" src="file.svg"/>
                    </div>
                    <div class="itemname">${diritem.name}</div>
                    `;
                }
                else if (lkmode == "->dir") {
                    item.innerHTML = `
                    <div class="container link">
                        <img class="icon" src="folder.svg"/>
                    </div>
                    <div class="itemname">${diritem.name}</div>
                    `;
                }
                else {
                    item.innerHTML = `
                    <div class="container link">
                        <img class="icon" src="unknown.svg"/>
                    </div>
                    <div class="itemname">${diritem.name}</div>
                    `;
                }

                item.className = "diritem";
                item.onclick = () => { onItemClicked(diritem) };
            }


            // item.innerText = vals[0];
            // item.onclick = () => {
            //     sendRequire("cd", vals[0]);
            // };
            div.append(item);
        }
    }
    else if (gcmd == "cat") {
        let ui = document.querySelector("#uicontainer");
        let editor = document.querySelector("#fileeditor");
        let filetile = document.querySelector("#filetitle");
        let editorarea = document.querySelector("#editorarea");

        editor.classList.remove("hide");
        ui.classList.add("hide");
        filetile.innerHTML = gval;



        let src = "";
        for (let idx in datas) {
            if (idx == 0) continue;
            if (idx == datas.length - 1) continue;
            src += datas[idx];
        }

        editorarea.value = src.slice(0, src.length - 1);



    }
    else if (gcmd == "cd" || gcmd == "mkdir" || gcmd == "save" || gcmd == "format" || gcmd == "rm" || gcmd == "rename"
        || gcmd == "cp" || gcmd == "chmod" || gcmd == "slk" || gcmd == "hlk" || gcmd != "") {
        sendRequire("ls");
    }
    else if (first) {
        first = false;
        sendRequire("ls");
    }

    // }
}

function onButtonClicked(buttonId) {
    // alert('点击' + buttonId);
    if (buttonId === 1) {
        // 创建 EventSource 对象，连接到服务器的 SSE 路由
        const evtSource = new EventSource('/osfs/restart');

        // 监听事件流，接收服务器端发送的实时输出
        // eventSource.onmessage = (event) => {
        //     console.log(1);
        //     const output = event.data;
        //     console.log(output)
        //     // 处理服务器端发送的实时输出
        // };



        evtSource.addEventListener("error", function (e) {
            console.log("服务器发送给客户端的数据为:" + e.data);
        });

        //只要和服务器连接，就会触发open事件
        evtSource.addEventListener("open", function () {
            console.log("和服务器建立连接");
        });

        //处理服务器响应报文中的load事件
        evtSource.addEventListener("load", function (e) {
            console.log(2222, e);
            var newElement = document.createElement("pre");
            let eventList = document.getElementsByTagName('body')[0]
            var obj = JSON.parse(e.data);
            newElement.innerHTML = "" + obj;
            eventList.appendChild(newElement);

        }, false);

        //处理服务器响应报文中的load事件
        evtSource.addEventListener("close", function (e) {
            console.log("结束:", e);
            evtSource.close();
        });


        // axios.post('idletrade/restart'
        //     , {})
        //     .then((successResponse) => {

        //         console.log(successResponse.data)
        //     })
        //     .catch((failResponse) => {
        //         alert(failResponse.response.data.message ? failResponse.response.data.message : failResponse.message);
        //     });
    }
    else if (buttonId === 2) {
        // 创建 EventSource 对象，连接到服务器的 SSE 路由
        const evtSource = new EventSource('/idletrade/update');

        // 监听事件流，接收服务器端发送的实时输出
        // eventSource.onmessage = (event) => {
        //     console.log(1);
        //     const output = event.data;
        //     console.log(output)
        //     // 处理服务器端发送的实时输出
        // };



        evtSource.addEventListener("error", function (e) {
            console.log("服务器发送给客户端的数据为:" + e.data);
        });

        //只要和服务器连接，就会触发open事件
        evtSource.addEventListener("open", function () {
            console.log("和服务器建立连接");
        });

        //处理服务器响应报文中的load事件
        evtSource.addEventListener("load", function (e) {
            console.log(2222, e);
            var newElement = document.createElement("pre");
            let eventList = document.getElementsByTagName('body')[0]
            var obj = JSON.parse(e.data);
            newElement.innerHTML = "" + obj;
            eventList.appendChild(newElement);

        }, false);

        //处理服务器响应报文中的load事件
        evtSource.addEventListener("close", function (e) {
            console.log("结束:", e);
            evtSource.close();
        });
    }

}



function exitEditor() {
    let ui = document.querySelector("#uicontainer");
    let editor = document.querySelector("#fileeditor");
    let filetile = document.querySelector("#filetitle");
    let editorarea = document.querySelector("#editorarea");

    ui.classList.remove("hide");
    editor.classList.add("hide");
    sendRequire("ls");
}


function onSaveClicked() {
    let filename = document.querySelector("#filetitle").innerHTML;
    let txt = document.querySelector("#editorarea").value;
    sendRequire("writer", `${filename} \n${txt}\x1b`);
    showSuccMessage("完成");
    

}




function initXterm() {
    var window_width = document.body.clientWidth;
    var window_height = document.body.clientHeight;
    var term = new Terminal(
        {
            cols: Math.floor(window_width / 10),
            rows: Math.floor(window_height / 25),
            useStyle: true,
            cursorBlink: true,
            theme: {
                foreground: "#fff", //字体
                background: "#010106", //背景色
                cursor: "#fff", //设置光标
                lineHeight: 25
            }
        }
    );

    // var term = new Terminal();
    term.open(document.getElementById('terminal'));





    gsocket = new WebSocket('/');

    gsocket.onopen = () => {
        term.write('连接已建立\r\n');
    };

    gsocket.onmessage = (event) => {
        // alert(event.data);
        term.write(event.data);
        acceptNewData(event.data);
    };

    term.onData(data => {
        gsocket.send(data);
    });

    gsocket.onclose = () => {
        term.write('连接已关闭\r\n');
    };

    gsocket.onerror = (error) => {
        term.write(`\r\nWebgsocket error: ${error.message}\r\n`);
    };

    // let term = new Terminal({
    //     rendererType: "canvas", //渲染类型
    //     rows: _this.rows, //行数
    //     cols: _this.cols, // 不指定行数，自动回车后光标从下一行开始
    //     convertEol: true, //启用时，光标将设置为下一行的开头
    //     // scrollback: 50, //终端中的回滚量
    //     disableStdin: false, //是否应禁用输入
    //     // cursorStyle: "underline", //光标样式
    //     cursorBlink: true, //光标闪烁
    //     theme: {
    //         foreground: "#ECECEC", //字体
    //         background: "#000000", //背景色
    //         cursor: "help", //设置光标
    //         lineHeight: 20
    //     }
    // })
    // // 创建terminal实例
    // term.open(this.$refs["terminal"])
    // // 换行并输入起始符 $
    // term.prompt = _ => {
    //     term.write("\r\n\x1b[33m$\x1b[0m ")
    // }
    // term.prompt()
    // // canvas背景全屏
    // const fitAddon = new FitAddon()
    // term.loadAddon(fitAddon)
    // fitAddon.fit()

    // window.addEventListener("resize", resizeScreen)
    // function resizeScreen() {
    //     try { // 窗口大小改变时，触发xterm的resize方法使自适应
    //         fitAddon.fit()
    //     } catch (e) {
    //         console.log("e", e.message)
    //     }
    // }
    // _this.term = term
    // _this.runFakeTerminal()
    // }
    // runFakeTerminal() {
    //     let _this = this
    //     let term = _this.term
    //     if (term._initialized) return
    //     // 初始化
    //     term._initialized = true
    //     term.writeln("Welcome to \x1b[1;32m墨天轮\x1b[0m.")
    //     term.writeln('This is Web Terminal of Modb; Good Good Study, Day Day Up.')
    //     term.prompt()
    //     // 添加事件监听器，支持输入方法
    //     term.onKey(e => {
    //         const printable = !e.domEvent.altKey && !e.domEvent.altGraphKey && !e.domEvent.ctrlKey && !e.domEvent.metaKey
    //         if (e.domEvent.keyCode === 13) {
    //             term.prompt()
    //         } else if (e.domEvent.keyCode === 8) { // back 删除的情况
    //             if (term._core.buffer.x > 2) {
    //                 term.write('\b \b')
    //             }
    //         } else if (printable) {
    //             term.write(e.key)
    //         }
    //         console.log(1, 'print', e.key)
    //     })
    //     term.onData(key => {  // 粘贴的情况
    //         if (key.length > 1) term.write(key)
    //     })
    // }
    // }
}

function newFolder() {
    let folderName = "newfolder";
    console.log(curDirs);
    if (curDirs.includes(folderName)) {
        let i = 1;
        while (curDirs.includes(`${folderName}(${i})`)) {
            i++;
        }
        sendRequire("mkdir", `${folderName}(${i})`);
    }
    else {
        sendRequire("mkdir", folderName);
    }
}


function newFile() {
    let folderName = "newFile";
    console.log(curDirs);
    if (curDirs.includes(folderName + ".txt")) {
        let i = 1;
        while (curDirs.includes(`${folderName}(${i}).txt`)) {
            i++;
        }
        sendRequire("save", `${folderName}(${i}).txt 0`);
    }
    else {
        sendRequire("save", `${folderName}.txt 0`);
    }
}




function onPasteClick() {
    // alert(copypath);
    document.querySelector("#paste").disabled = true;
    // alert(path);
    sendRequire("cp", copypath + " " + path);
}


// function ls() {
//     sendRequire("ls");
// }


function format() {
    let ans = prompt("确定吗？您的数据将丢失！", "确定");
    if (ans != "确定") {
        return;
    }
    sendRequire("format", "-y\nloaddata\n");
    alert("完成");
}

function laodexample() {
    let ans = prompt("确定吗？", "确定");
    if (ans != "确定") {
        return;
    }
    sendRequire("format", "\ny\nloaddata\n");
    alert("完成");
}


function logout() {
    let namae = document.querySelector("#username").innerHTML;
    sendRequire("logout", "\n" + namae + "\n");
    alert("已退出登录");
}


function newPicture() {

    // 点击图片的同时，点击上传文件的input
    let input = document.querySelector(".file-upload");
    input.click();
    input.onchange = () => {
        var reader = new FileReader();
        // 转换成功后的操作，img.src即为转换后的DataURL
        reader.onload = function (e) {
            if (reader.readyState === 2) { //加载完毕后赋值
                // img.src = e.target.result
                // console.log(input.files[0].name)
                sendRequire("writer", `${input.files[0].name}.src \n${e.target.result}\x1b`);
            }
        }
        reader.readAsDataURL(input.files[0]);
    }
}

function login() {
    let name = prompt("请输入要登录的用户名：");
    let passwd = prompt("请输入密码：", "", {
        confirmButtonText: '确定',
        cancelButtonText: '取消',
        inputType: 'password',
    });
    if (name == null)
        return;
    if (passwd == null)
        return;
    if (name != "" && passwd != "")
        sendRequire("login", "\n" + name + "\n" + passwd + "\n");
    return;
}

function refresh() {
    sendRequire("ls");
    showSuccMessage("已刷新");
}

initXterm();

