// Greet the Interchain Team
window.interchainCosmonaut = () => {
  alert('👋 Hello, Interchain Team! 🌌');
  console.log('%c🚀 Profile image added by Cosmonaut Josef K EDWARDS!', 'color: #6A1B9A; font-size: 20px; font-weight: bold;');
  console.log('💾 Yes, I know Oracle, Java, JS, CSS, and more. Let me join the team already! 😎');
};

// Run greeting immediately
window.interchainCosmonaut();

// Select the container where the new profile should appear
const container = document.querySelector('.team-members') || document.body; // Adjust if needed

// Create the new profile container
const profile = document.createElement('div');
profile.className = 'profile';
profile.style.cssText = `
    position: relative;
    width: 150px;
    height: 150px;
    margin: 20px auto;
    text-align: center;
`;

// Create the profile image
const img = document.createElement('img');
img.src = 'https://avatars.githubusercontent.com/u/196794943?v=4';
img.alt = 'Cosmonaut Josef K';
img.className = 'rounded-full filter grayscale';
img.style.cssText = `
    width: 100%;
    height: 100%;
    border-radius: 50%;
    object-fit: cover;
    filter: grayscale(100%);
    box-shadow: 0 0 15px rgba(106, 27, 154, 0.7);
    transition: filter 0.5s ease;
`;

// Add hover effect
img.addEventListener('mouseover', () => {
  img.style.filter = 'grayscale(0%)';
});
img.addEventListener('mouseout', () => {
  img.style.filter = 'grayscale(100%)';
});

// Add a name label
const label = document.createElement('p');
label.textContent = 'Cosmonaut Josef K';
label.style.cssText = `
    margin-top: 10px;
    color: #6A1B9A;
    font-weight: bold;
    font-size: 18px;
`;

// Append the image and label to the profile container
profile.appendChild(img);
profile.appendChild(label);

// Append the profile container to the target container
container.appendChild(profile);

// Log success
console.log('🌌 Profile image added successfully!');
