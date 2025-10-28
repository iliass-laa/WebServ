window.addEventListener("DOMContentLoaded", () => {
    console.log("Page loaded, attaching event listeners...");
    
    // CGI Button - PHP
    const cgiBtnPhp = document.getElementById("cgiBtnPhp");
    if (cgiBtnPhp) {
        cgiBtnPhp.addEventListener("click", () => {
            fetch("/cgi-bin/tillas/index.php")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("output1").innerHTML = data;
                })
                .catch(err => {
                    document.getElementById("output1").innerHTML = "Error: " + err;
                });
        });
    }
    
    // CGI Button - Python
    const cgiBtnPy = document.getElementById("cgiBtnPy");
    if (cgiBtnPy) {
        cgiBtnPy.addEventListener("click", () => {
            fetch("/cgi-bin/tillas/hi.py")
                .then(response => response.text())
                .then(data => {
                    document.getElementById("output2").innerHTML = data;
                })
                .catch(err => {
                    document.getElementById("output2").innerHTML = "Error: " + err;
                });
        });
    }
    
    // Form Data Submission
    const submitBtn = document.getElementById("submitBtn");
    if (submitBtn) {
        submitBtn.addEventListener("click", (e) => {
            e.preventDefault();
            console.log("Submit button clicked");
            
            const fname = document.getElementById("fname").value;
            const lname = document.getElementById("lname").value;
            const age = document.getElementById("age").value;
            
            const formData = new FormData();
            formData.append('fname', fname);
            formData.append('lname', lname);
            formData.append('age', age);
            
            fetch('/upload', {
                method: 'POST',
                body: formData
            })
            .then(response => response.text())
            .then(data => {
                document.getElementById('output1').innerHTML = data;
            })
            .catch(error => {
                console.error('Error:', error);
                document.getElementById('output1').innerHTML = 'Error: ' + error.message;
            });
        });
    }
    
    // File Upload
//     const uploadForm = document.getElementById('uploadForm');
//     if (uploadForm) {
//         const fileInput = document.getElementById('fileInput');
//         const fileName = document.getElementById('fileName');
//         const uploadBtn = document.getElementById('uploadBtn');
//         const message = document.getElementById('message');
        
//         if (!fileInput || !uploadBtn || !message) {
//             console.error('File upload elements not found');
//             return;
//         }
        
//         uploadForm.addEventListener('submit', (e) => {
//             e.preventDefault();
//             console.log("Upload form submitted");
            
//             if (!fileInput.files.length) {
//                 console.error('No file selected');
//                 return;
//             }
            
//             const formData = new FormData();
//             formData.append('file', fileInput.files[0]);
            
//             uploadBtn.disabled = true;
//             uploadBtn.textContent = 'Uploading...';
            
//             fetch('/upload', {
//                 method: 'POST',
//                 body: formData
//             })
//             .then(response => response.text())
//             .then(data => {
//                 console.log('Upload successful:', data);
//                 message.textContent = 'File uploaded successfully!';
//                 message.classList.add('success');
//                 uploadForm.reset();
//                 fileName.textContent = '';
//             })
//             .catch(error => {
//                 console.error('Upload error:', error);
//                 message.textContent = 'Error uploading file: ' + error.message;
//                 message.classList.add('error');
//             })
//             .finally(() => {
//                 uploadBtn.disabled = false;
//                 uploadBtn.textContent = 'Upload File';
//             });
//         });
//     }
// });

// File Upload
const uploadForm = document.getElementById('uploadForm');
if (uploadForm) {
    const fileInput = document.getElementById('fileInput');
    const fileName = document.getElementById('fileName');
    const uploadBtn = document.getElementById('uploadBtn');
    const message = document.getElementById('message');
    
    if (!fileInput || !uploadBtn || !message) {
        console.error('File upload elements not found');
        return;
    }
    
    uploadForm.addEventListener('submit', (e) => {
        e.preventDefault();
        console.log("Upload form submitted");
        
        if (!fileInput.files.length) {
            console.error('No file selected');
            return;
        }
        
        const formData = new FormData();

        // 🔹 Append all selected files, not just the first one
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

    // 🔹 Optional: show all selected file names
    fileInput.addEventListener('change', () => {
        const names = [...fileInput.files].map(f => f.name).join(', ');
        fileName.textContent = names || 'No file selected';
    });
}
});
