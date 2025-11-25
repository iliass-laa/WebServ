const form = document.getElementById('loginForm');
        const responseDiv = document.getElementById('response');
        
        form.addEventListener('submit', async (e) => {
            e.preventDefault();
            
            const formData = new FormData(form);
            
            try {
                const response = await fetch(form.action, {
                    method: 'POST',
                    body: formData
                });
                
                if (response.ok) {
                    responseDiv.textContent = 'Login successful! Redirecting...';
                    responseDiv.className = 'response success';
                    responseDiv.style.display = 'block';
                    form.reset();
                } else {
                    responseDiv.textContent = 'Invalid credentials. Please try again.';
                    responseDiv.className = 'response error';
                    responseDiv.style.display = 'block';
                }
            } catch (error) {
                responseDiv.textContent = 'Login failed. Please try again.';
                responseDiv.className = 'response error';
                responseDiv.style.display = 'block';
            }
        });