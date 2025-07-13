function getBackendDomain() {
    const domain = window.location.hostname || window.currentURL.hostname;
    const firstDotIndex = domain.indexOf('.');
    const subdomain = domain.substring(0, firstDotIndex);
    const restOfDomain = domain.substring(firstDotIndex);
    return "https://" + subdomain + "-backend" + restOfDomain;
}

async function getWeather() {
    const city = document.getElementById('cityInput').value;
    if (!city) {
        alert("Please enter a city name!");
        return;
    }

    try {
        const api = getBackendDomain();
        
        // Use Axios for API request
        const response = await axios.post(api + '/weather', { city: city });
        const data = response.data;
        
        if (data.error) {
            document.getElementById('weatherResult').innerHTML = 
                `<p style="color: red;">${data.error}</p>`;
        } else {
            document.getElementById('weatherResult').innerHTML = `
                <h2>Weather in ${data.City}</h2>
                <p>Temperature: ${data.Temperature}°C</p>
                <p>Description: ${data.Description}</p>
                <p>Humidity: ${data.Humidity}%</p>
            `;
        }
    } catch (error) {
        console.error("Error:", error);
    }
}