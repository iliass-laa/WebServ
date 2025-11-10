window.addEventListener("DOMContentLoaded", () => {
    const btn = document.getElementById("cgiBtnPhp");
    const output = document.getElementById("output1");

    btn.addEventListener("click", () => {
        fetch("/cgi-bin/index.php")
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
        fetch("/cgi-bin/sayHi.py")
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
    const btn = document.getElementById("cgiBtnPySleep");
    const output = document.getElementById("output6");

    btn.addEventListener("click", () => {
        fetch("/cgi-bin/sleeping.py")
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
    const btn = document.getElementById("cgiBtnImg");
    const output = document.getElementById("output3");

    btn.addEventListener("click", () => {
        fetch("/cgi-bin/sendImgPhp.php")
            .then(response => response.blob())  // get binary data as blob
            .then(blob => {
                const imgURL = URL.createObjectURL(blob); // create temporary URL
                output.innerHTML = `<img src="${imgURL}" alt="CGI Image" style="max-width: 300px;">`;
            })
            .catch(err => {
                output.innerHTML = "Error: " + err;
            });
    });
});


window.addEventListener("DOMContentLoaded", () => {
    const btn = document.getElementById("downloadBtn");
    const output = document.getElementById("outputOfDownload");

    btn.addEventListener("click", () => {
        fetch("/cgi-bin/sendVidPhp.php")
            .then(response => response.blob())  // get binary data as blob
            .then(blob => {
                const imgURL = URL.createObjectURL(blob); // create temporary URL
                output.innerHTML = `<h2>Downloaded successfully</h2>`;
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
                fetch('/cgi-bin/prog', {
                    method: 'POST',
                    body: formData
                })
                .then(response => response.text())
                .then(data => {
                    // Display response in output div
                    document.getElementById('outputOfSubmit').innerHTML = data;
                })
                .catch(error => {
                    console.error('Error:', error);
                    document.getElementById('outputOfSubmit').innerHTML = 'Error: ' + error.message;
                });
            });
        });
// ✅ FIXED - waits for DOM
window.addEventListener("DOMContentLoaded", () => {
    const uploadForm = document.getElementById('uploadForm');
    const fileInput = document.getElementById('fileInput');
    const uploadBtn = document.getElementById('uploadBtn');
    const message = document.getElementById('message');
    const fileName = document.getElementById('fileName');

    uploadForm.addEventListener('submit', (e) => {
        e.preventDefault();
        console.log("Upload form submitted");

        if (!fileInput.files.length) {
            console.error('No file selected');
            return;
        }

        const formData = new FormData();

        for (const file of fileInput.files) {
            formData.append('files[]', file);
        }

        uploadBtn.disabled = true;
        uploadBtn.textContent = 'Uploading...';

        fetch('/upload', {
            method: 'POST',
            body: formData
        })
        .then(response => response.text())
        .then(data => {
            console.log('Upload successful:', data);
            message.textContent = 'Files uploaded successfully!';
            message.classList.add('success');
            uploadForm.reset();
            fileName.textContent = '';
        })
        .catch(error => {
            console.error('Upload error:', error);
            message.textContent = 'Error uploading files: ' + error.message;
            message.classList.add('error');
        })
        .finally(() => {
            uploadBtn.disabled = false;
            uploadBtn.textContent = 'Upload File';
        });
    });
});