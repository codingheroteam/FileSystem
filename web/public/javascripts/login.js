

function onLoginClicked() {

    let account = document.querySelector('#account').value;
    let password = document.querySelector('#password').value;
    axios.post('login/api'
        , {
            account: account,
            password: password,
        })
        .then((successResponse) => {

            if (successResponse.status === 200) {
                setTimeout(() => {
                    window.location.replace('/');
                })
            }
        })
        .catch((failResponse) => {
            alert(failResponse.response.data.message?failResponse.response.data.message:failResponse.message);
        });
}

