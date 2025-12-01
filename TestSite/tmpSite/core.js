// ✅ Single DOMContentLoaded listener for all functionality
window.addEventListener("DOMContentLoaded", () => {
    
    // ========== CGI Button Handlers ==========
    
     
    
    
    // __________Log In __________ //
    // const LogInBtn = document.getElementById("LogIn");

    // LogInBtn.addEventListener("click", ()=>{
    //     fetch("/cgi-bin/logIn")
    //     .then(response =>response.text())
    //     .catch(err => {
    //         console.log(err)
    //     })
    // })

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
    

    // const showInfo = document.getElementById("showInfo");
    // showInfo.addEventListener("click", ()=>{
    //     fetch("/cgi-bin/printinfo")
    // //     const response = await fetch("/cgi-bin/printinfo");
    // // const data = await response.text();
    // // alert(data);
    // })

    showInfo.addEventListener("click", async () => {
    const response = await fetch("/cgi-bin/showInfos");
    const data = await response.text();
    alert(data);
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
        fetch('/cgi-bin/auth', {
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

    // uploadForm.addEventListener('submit', (e) => {
    //     e.preventDefault();
    //     console.log("Upload form submitted");

    //     if (!fileInput.files.length) {
    //         message.textContent = 'Please select at least one file';
    //         message.className = 'message error';
    //         console.error('No file selected');
    //         return;
    //     }

    //     const formData = new FormData();

    //     for (const file of fileInput.files) {
    //         formData.append('files[]', file);
    //     }

    //     uploadBtn.disabled = true;
    //     uploadBtn.textContent = 'Uploading...';
    //     message.className = 'message';
    //     message.textContent = '';

    //     fetch('/upload', {
    //         method: 'POST',
    //         body: formData
    //     })
    //     .then(response => response.text())
    //     .then(data => {
    //         console.log('Upload successful:', data);
    //         message.textContent = 'Files uploaded successfully!';
    //         message.className = 'message success';
    //         uploadForm.reset();
    //         fileName.textContent = '';
    //     })
    //     .catch(error => {
    //         console.error('Upload error:', error);
    //         message.textContent = 'Error uploading files: ' + error.message;
    //         message.className = 'message error';
    //     })
    //     .finally(() => {
    //         uploadBtn.disabled = false;
    //         uploadBtn.textContent = 'Upload File';
    //     });
    // });

    // Store uploaded file names
let uploadedFiles = [];
let selectedFiles = new Set();

// Handle file input change (show selected file names)
fileInput.addEventListener('change', function(e) {
    const files = Array.from(e.target.files);
    
    if (files.length > 0) {
        const names = files.map(f => f.name).join(', ');
        fileName.textContent = `${files.length} file(s) selected: ${names}`;
    } else {
        fileName.textContent = '';
    }
});

// UPLOAD FORM HANDLER
uploadForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    console.log("Upload form submitted");

    if (!fileInput.files.length) {
        message.textContent = 'Please select at least one file';
        message.className = 'message error';
        console.error('No file selected');
        return;
    }

    const formData = new FormData();
    const filesToAdd = Array.from(fileInput.files);

    for (const file of filesToAdd) {
        formData.append('files[]', file);
    }

    uploadBtn.disabled = true;
    uploadBtn.textContent = 'Uploading...';
    message.className = 'message';
    message.textContent = '';

    try {
        const response = await fetch('/upload', {
            method: 'POST',
            body: formData
        });
        
        const data = await response.text();
        console.log('Upload successful:', data);
        
        message.textContent = 'Files uploaded successfully!';
        message.className = 'message success';
        
        // JUST REMEMBER THE FILE NAMES
        const newFiles = filesToAdd.map(file => ({
            name: file.name,
            size: file.size,
            type: file.type,
            uploadedAt: new Date().toISOString()
        }));
        
        uploadedFiles = [...uploadedFiles, ...newFiles];
        renderFileList();
        
        // Reset form
        uploadForm.reset();
        fileName.textContent = '';
        
    } catch (error) {
        console.error('Upload error:', error);
        message.textContent = 'Error uploading files: ' + error.message;
        message.className = 'message error';
    } finally {
        uploadBtn.disabled = false;
        uploadBtn.textContent = 'Upload File';
    }
});

// Render the file list
function renderFileList() {
    const fileList = document.getElementById('fileList');
    const fileCount = document.getElementById('fileCount');
    const deleteSelectedBtn = document.getElementById('deleteSelectedBtn');
    
    fileCount.textContent = uploadedFiles.length;

    if (uploadedFiles.length === 0) {
        fileList.innerHTML = `
            <div class="empty-state">
                <p>📁 No files uploaded yet</p>
            </div>
        `;
        deleteSelectedBtn.style.display = 'none';
        return;
    }

    fileList.innerHTML = uploadedFiles.map((file, index) => `
        <div class="file-item ${selectedFiles.has(index) ? 'selected' : ''}" data-index="${index}">
            <input 
                type="checkbox" 
                class="file-checkbox" 
                ${selectedFiles.has(index) ? 'checked' : ''}
                onchange="toggleFileSelection(${index})"
            >
            <div class="file-icon">
                ${getFileIcon(file.type)}
            </div>
            <div class="file-info">
                <div class="file-name">${escapeHtml(file.name)}</div>
                <div class="file-meta">
                    ${formatFileSize(file.size)} • ${formatDate(file.uploadedAt)}
                </div>
            </div>
            <button class="delete-btn" onclick="deleteFile(${index}, '${escapeHtml(file.name)}')">
                Delete
            </button>
        </div>
    `).join('');

    deleteSelectedBtn.style.display = selectedFiles.size > 0 ? 'block' : 'none';
}

// Toggle file selection
function toggleFileSelection(index) {
    if (selectedFiles.has(index)) {
        selectedFiles.delete(index);
    } else {
        selectedFiles.add(index);
    }
    renderFileList();
}

// Delete a single file - SENDS DELETE REQUEST TO SERVER
async function deleteFile(index, filename) {
    if (!confirm(`Are you sure you want to delete ${filename}?`)) {
        return;
    }
    
    try {
        const response = await fetch(`/delete/${encodeURIComponent(filename)}`, {
            method: 'DELETE'
        });
        
        if (response.ok) {
            console.log('Delete successful');
            uploadedFiles.splice(index, 1);
            selectedFiles.delete(index);
            renderFileList();
            
            // Show success message
            message.textContent = `File "${filename}" deleted successfully!`;
            message.className = 'message success';
        } else {
            throw new Error('Delete failed');
        }
    } catch (error) {
        console.error('Delete error:', error);
        message.textContent = `Error deleting file: ${error.message}`;
        message.className = 'message error';
    }
}

// Delete selected files - SENDS MULTIPLE DELETE REQUESTS
document.getElementById('deleteSelectedBtn').addEventListener('click', async function() {
    if (selectedFiles.size === 0) return;
    
    const selectedIndexes = Array.from(selectedFiles).sort((a, b) => b - a); // Sort descending
    const filesToDelete = selectedIndexes.map(i => uploadedFiles[i].name);
    
    if (!confirm(`Are you sure you want to delete ${filesToDelete.length} file(s)?`)) {
        return;
    }
    
    let successCount = 0;
    let failCount = 0;
    
    for (const filename of filesToDelete) {
        try {
            const response = await fetch(`/delete/${encodeURIComponent(filename)}`, {
                method: 'DELETE'
            });
            
            if (response.ok) {
                successCount++;
            } else {
                failCount++;
            }
        } catch (error) {
            console.error(`Error deleting ${filename}:`, error);
            failCount++;
        }
    }
    
    // Remove deleted files from array
    for (const index of selectedIndexes) {
        uploadedFiles.splice(index, 1);
    }
    
    selectedFiles.clear();
    renderFileList();
    
    // Show result message
    if (failCount === 0) {
        message.textContent = `Successfully deleted ${successCount} file(s)!`;
        message.className = 'message success';
    } else {
        message.textContent = `Deleted ${successCount} file(s), ${failCount} failed`;
        message.className = 'message error';
    }
});

// Helper functions
function getFileIcon(type) {
    if (type.startsWith('image/')) return '🖼️';
    if (type.startsWith('video/')) return '🎬';
    if (type.startsWith('text/')) return '📄';
    return '📎';
}

function formatFileSize(bytes) {
    if (bytes === 0) return '0 Bytes';
    const k = 1024;
    const sizes = ['Bytes', 'KB', 'MB', 'GB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return Math.round(bytes / Math.pow(k, i) * 100) / 100 + ' ' + sizes[i];
}

function formatDate(dateString) {
    const date = new Date(dateString);
    return date.toLocaleDateString() + ' ' + date.toLocaleTimeString();
}

function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}
});