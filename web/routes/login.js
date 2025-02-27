var express = require('express');
var router = express.Router();
var session = require('express-session');
/* GET home page. */
router.get('/', function (req, res, next) {
    res.render('login', { title: 'Express' });
});


router.post('/api', function (req, res, next) {
    var sess = req.session;
    var params = req.body;
    const account = params.account;
    const password = params.password;
    console.log('账号', account, '密码', password);
    if (account == 'brayden' && password == 'qwemnb123.') {
        console.log(1);
        try{
        req.session.loginUserID = account;
        }
        catch(e){
            console.log(e);
        }
        console.log(2);
        resSuccSend(res, '成功')
        console.log(3);
    }
    else {
        resErrSend(res, ErrCode.LOGIN_FAILD);
    }

});


module.exports = router;
