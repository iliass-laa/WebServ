// window.addEventListener("DOMContentLoaded", () => {
//     const btn = document.getElementById("cgiBtnPhp");
//     const output = document.getElementById("output1");

//     btn.addEventListener("click", () => {
//         fetch("/cgi-bin/tillas/index.php")
//             .then(response => response.text())
//             .then(data => {
//                 output.innerHTML = data;
//             })
//             .catch(err => {
//                 output.innerHTML = "Error: " + err;
//             });
//     });
// });


// window.addEventListener("DOMContentLoaded", () => {
//     const btn = document.getElementById("cgiBtnPy");
//     const output = document.getElementById("output2");

//     btn.addEventListener("click", () => {
//         fetch("/cgi-bin/tillas/hi.py")
//             .then(response => response.text())
//             .then(data => {
//                 output.innerHTML = data;
//             })
//             .catch(err => {
//                 output.innerHTML = "Error: " + err;
//             });
//     });
// });


//  window.addEventListener("DOMContentLoaded", () => {
//             const btn = document.getElementById("uploadBtn");
//             btn.addEventListener("click", () => {
//                 // Get form data
//                 const fname = document.getElementById("fname").value;
//                 const lname = document.getElementById("lname").value;
//                 const age = document.getElementById("age").value;
                
//                 // Create form data object
//                 const formData = new FormData();
//                 formData.append('fname', fname);
//                 formData.append('lname', lname);
//                 formData.append('age', age);
                
//                 // Send POST request
//                 fetch('/upload', {
//                     method: 'POST',
//                     body: formData
//                 })
//                 .then(response => response.text())
//                 .then(data => {
//                     // Display response in output div
//                     document.getElementById('output1').innerHTML = data;
//                 })
//                 .catch(error => {
//                     console.error('Error:', error);
//                     document.getElementById('output1').innerHTML = 'Error: ' + error.message;
//                 });
//             });
//         });







// const fileInput = document.getElementById('fileInput');
// const fileName = document.getElementById('fileName');
// const uploadForm = document.getElementById('uploadForm');
// const uploadBtn = document.getElementById('uploadBtn');
// const message = document.getElementById('message');
// const fileInputLabel = document.querySelector('.file-input-label');

// // Display selected file name
// fileInput.addEventListener('change', function() {
//     if (this.files.length > 0) {
//         fileName.textContent = 'File: ' + this.files[0].name;
//     } else {
//         fileName.textContent = '';
//     }
// });

// // Drag and drop functionality
// fileInputLabel.addEventListener('dragover', function(e) {
//     e.preventDefault();
//     this.classList.add('active');
// });

// fileInputLabel.addEventListener('dragleave', function(e) {
//     e.preventDefault();
//     this.classList.remove('active');
// });

// fileInputLabel.addEventListener('drop', function(e) {
//     e.preventDefault();
//     this.classList.remove('active');
    
//     const files = e.dataTransfer.files;
//     if (files.length > 0) {
//         fileInput.files = files;
//         fileName.textContent = 'File: ' + files[0].name;
//     }
// });

// // Form submission
// uploadForm.addEventListener('submit', async function(e) {
//     e.preventDefault();
    
//     // Hide previous message
//     message.classList.remove('success', 'error');
//     message.textContent = '';
    
//     // Validate file selection
//     if (fileInput.files.length === 0) {
//         showMessage('Please select a file first', 'error');
//         return;
//     }
    
//     // Create FormData object
//     const formData = new FormData();
//     formData.append('file', fileInput.files[0]);
    
//     // Disable submit button
//     uploadBtn.disabled = true;
//     uploadBtn.textContent = 'Uploading...';
    
//     try {
//         const response = await fetch('/upload', {
//             method: 'POST',
//             body: formData
//         });
        
//         if (response.ok) {
//             showMessage('File uploaded successfully!', 'success');
//             uploadForm.reset();
//             fileName.textContent = '';
//         } else {
//             showMessage('Upload failed: ' + response.statusText, 'error');
//         }
//     } catch (error) {
//         showMessage('Error uploading file: ' + error.message, 'error');
//     } finally {
//         uploadBtn.disabled = false;
//         uploadBtn.textContent = 'Upload File';
//     }
// });

// function showMessage(text, type) {
//     message.textContent = text;
//     message.classList.add(type);
// }

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
            formData.append('file', fileInput.files[0]);
            
            uploadBtn.disabled = true;
            uploadBtn.textContent = 'Uploading...';
            
            fetch('/upload', {
                method: 'POST',
                body: formData
            })
            .then(response => response.text())
            .then(data => {
                console.log('Upload successful:', data);
                message.textContent = 'File uploaded successfully!';
                message.classList.add('success');
                uploadForm.reset();
                fileName.textContent = '';
            })
            .catch(error => {
                console.error('Upload error:', error);
                message.textContent = 'Error uploading file: ' + error.message;
                message.classList.add('error');
            })
            .finally(() => {
                uploadBtn.disabled = false;
                uploadBtn.textContent = 'Upload File';
            });
        });
    }
});