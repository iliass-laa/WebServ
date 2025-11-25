// ✅ Single DOMContentLoaded listener for all functionality
window.addEventListener("DOMContentLoaded", () => {
    
    // ========== CGI Button Handlers ==========
    
     // ======= Cookie Btn Check:

    const CookieBtn = document.getElementById("checkCookie");
    const OutputOfCookieCheck =document.getElementById("outputCookie");

    CookieBtn.addEventListener("click", ()=>{
        fetch("/cgi-bin/checkCookie")
        .then(response=> response.text())
        .then(data=> {

            OutputOfCookieCheck.innerHTML = data;
        })
        .catch (err => {
            OutputOfCookieCheck.innerHTML = "Error :" + err;
        })
    })
    // ============= set a Cookie that says i am Said
    const Saidbtn = document.getElementById("imSaid");
    Saidbtn.addEventListener("click", ()=>{
        fetch("/cgi-bin/said")
    })
    

     const aaa = document.getElementById("aaa");
    aaa.addEventListener("click", ()=>{
        fetch("/cgi-bin/sendVidPhp.php")
    })
    
    // PHP Script Button
    const cgiBtnPhp = document.getElementById("cgiBtnPhp");
    const output1 = document.getElementById("output1");
    
    cgiBtnPhp.addEventListener("click", () => {
        fetch("/cgi-bin/aaa")
            .then(response => response.text())
            .then(data => {
                output1.innerHTML = data;
            })
            .catch(err => {
                output1.innerHTML = "Error: " + err;
            });
    });

    // Python Script Button
    const cgiBtnPy = document.getElementById("cgiBtnPy");
    const output2 = document.getElementById("output2");
    
    cgiBtnPy.addEventListener("click", () => {
        fetch("/cgi-bin/sayHi.py")
            .then(response => response.text())
            .then(data => {
                output2.innerHTML = data;
            })
            .catch(err => {
                output2.innerHTML = "Error: " + err;
            });
    });

    // Python Sleep Script Button
    const cgiBtnPySleep = document.getElementById("cgiBtnPySleep");
    const output6 = document.getElementById("output6");
    
    cgiBtnPySleep.addEventListener("click", () => {
        fetch("/cgi-bin/sleeping.py")
            .then(response => response.text())
            .then(data => {
                output6.innerHTML = data;
            })
            .catch(err => {
                output6.innerHTML = "Error: " + err;
            });
    });

    // Image Script Button
    const cgiBtnImg = document.getElementById("cgiBtnImg");
    const output3 = document.getElementById("output3");
    
    cgiBtnImg.addEventListener("click", () => {
        fetch("/cgi-bin/sendImgPhp.php")
            .then(response => response.blob())
            .then(blob => {
                const imgURL = URL.createObjectURL(blob);
                output3.innerHTML = `<img src="${imgURL}" alt="CGI Image" style="max-width: 300px;">`;
            })
            .catch(err => {
                output3.innerHTML = "Error: " + err;
            });
    });

    // ========== Form Submission Handler ==========
    
    const submitBtn = document.getElementById("submitBtn");
    
    submitBtn.addEventListener("click", () => {
        const fname = document.getElementById("fname").value;
        const lname = document.getElementById("lname").value;
        const age = document.getElementById("age").value;
        
        const formData = new FormData();
        formData.append('fname', fname);
        formData.append('lname', lname);
        formData.append('age', age);
        
        // fetch('/cgi-bin/prog', {
        fetch('/cgi-bin/goTest', {
            method: 'POST',
            body: formData
        })
        .then(response => response.text())
        .then(data => {
            document.getElementById('outputOfSubmit').innerHTML = data;
        })
        .catch(error => {
            console.error('Error:', error);
            document.getElementById('outputOfSubmit').innerHTML = 'Error: ' + error.message;
        });
    });

    // ========== Download Button Handler ==========
    
    const downloadBtn = document.getElementById("downloadBtn");
    const outputOfDownload = document.getElementById("outputOfDownload");
    
    downloadBtn.addEventListener("click", () => {
        fetch("/cgi-bin/sendVidPhp.php")
            .then(response => {
                if (!response.ok) throw new Error('Download failed');
                return response.blob();
            })
            .then(blob => {
                // Create download link
                const url = URL.createObjectURL(blob);
                const a = document.createElement('a');
                a.href = url;
                a.download = 'video.mp4'; // Change extension based on your file type
                document.body.appendChild(a);
                a.click();
                document.body.removeChild(a);
                URL.revokeObjectURL(url);
                
                outputOfDownload.innerHTML = `<h2>Downloaded successfully</h2>`;
            })
            .catch(err => {
                outputOfDownload.innerHTML = "Error: " + err;
            });
    });

    // ========== File Upload Handler ==========
    
    const uploadForm = document.getElementById('uploadForm');
    const fileInput = document.getElementById('fileInput');
    const uploadBtn = document.getElementById('uploadBtn');
    const message = document.getElementById('message');
    const fileName = document.getElementById('fileName');

    // Show selected file names
    fileInput.addEventListener('change', () => {
        if (fileInput.files.length > 0) {
            const names = Array.from(fileInput.files).map(f => f.name).join(', ');
            fileName.textContent = names;
        } else {
            fileName.textContent = '';
        }
    });

    uploadForm.addEventListener('submit', (e) => {
        e.preventDefault();
        console.log("Upload form submitted");

        if (!fileInput.files.length) {
            message.textContent = 'Please select at least one file';
            message.className = 'message error';
            console.error('No file selected');
            return;
        }

        const formData = new FormData();

        for (const file of fileInput.files) {
            formData.append('files[]', file);
        }

        uploadBtn.disabled = true;
        uploadBtn.textContent = 'Uploading...';
        message.className = 'message';
        message.textContent = '';

        fetch('/upload', {
            method: 'POST',
            body: formData
        })
        .then(response => response.text())
        .then(data => {
            console.log('Upload successful:', data);
            message.textContent = 'Files uploaded successfully!';
            message.className = 'message success';
            uploadForm.reset();
            fileName.textContent = '';
        })
        .catch(error => {
            console.error('Upload error:', error);
            message.textContent = 'Error uploading files: ' + error.message;
            message.className = 'message error';
        })
        .finally(() => {
            uploadBtn.disabled = false;
            uploadBtn.textContent = 'Upload File';
        });
    });
});