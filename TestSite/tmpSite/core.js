window.addEventListener("DOMContentLoaded", () => {
    const btn = document.getElementById("cgiBtnPhp");
    const output = document.getElementById("output1");

    btn.addEventListener("click", () => {
        fetch("/cgi-bin/tillas/index.php")
            .then(response => response.text())
            .then(data => {
                output.innerHTML = data;
            })
            .catch(err => {
                output.innerHTML = "Error: " + err;
            });
    });
});


window.addEventListener("DOMContentLoaded", () => {
    const btn = document.getElementById("cgiBtnPy");
    const output = document.getElementById("output2");

    btn.addEventListener("click", () => {
        fetch("/cgi-bin/tillas/hi.py")
            .then(response => response.text())
            .then(data => {
                output.innerHTML = data;
            })
            .catch(err => {
                output.innerHTML = "Error: " + err;
            });
    });
});


 window.addEventListener("DOMContentLoaded", () => {
            const btn = document.getElementById("submitBtn");
            btn.addEventListener("click", () => {
                // Get form data
                const fname = document.getElementById("fname").value;
                const lname = document.getElementById("lname").value;
                const age = document.getElementById("age").value;
                
                // Create form data object
                const formData = new FormData();
                formData.append('fname', fname);
                formData.append('lname', lname);
                formData.append('age', age);
                
                // Send POST request
                fetch('/submit', {
                    method: 'POST',
                    body: formData
                })
                .then(response => response.text())
                .then(data => {
                    // Display response in output div
                    document.getElementById('output1').innerHTML = data;
                })
                .catch(error => {
                    console.error('Error:', error);
                    document.getElementById('output1').innerHTML = 'Error: ' + error.message;
                });
            });
        });