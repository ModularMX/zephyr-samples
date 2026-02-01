// Fetch device information and display it
async function fetchDeviceInfo() {
	try {
		const response = await fetch("/device-info");
		if (!response.ok) {
			throw new Error(`HTTP error! status: ${response.status}`);
		}

		const data = await response.json();

		// Update device info elements
		document.getElementById("board").textContent = data.board || "STM32H573I-DK";
		document.getElementById("arch").textContent = data.arch || "ARM Cortex-M33";
		document.getElementById("uptime").textContent = formatUptime(data.uptime);
		document.getElementById("status").textContent = data.status || "Running";
		document.getElementById("lastUpdate").textContent =
			"Last updated: " + new Date().toLocaleTimeString();

	} catch (error) {
		console.error("Failed to fetch device info:", error);
		document.getElementById("board").textContent = "Error";
	}
}

// Format uptime from milliseconds to readable format
function formatUptime(milliseconds) {
	const seconds = Math.floor(milliseconds / 1000);
	const minutes = Math.floor(seconds / 60);
	const hours = Math.floor(minutes / 60);
	const days = Math.floor(hours / 24);

	if (days > 0) {
		return `${days}d ${hours % 24}h ${minutes % 60}m`;
	} else if (hours > 0) {
		return `${hours}h ${minutes % 60}m ${seconds % 60}s`;
	} else if (minutes > 0) {
		return `${minutes}m ${seconds % 60}s`;
	} else {
		return `${seconds}s`;
	}
}

// Send echo request
async function sendEcho(message) {
	if (!message.trim()) return;

	try {
		const response = await fetch("/echo", {
			method: "POST",
			body: message
		});

		if (!response.ok) {
			throw new Error(`HTTP error! status: ${response.status}`);
		}

		const responseText = await response.text();
		document.getElementById("echoOutput").style.display = "block";
		document.getElementById("echoValue").textContent = responseText;
	} catch (error) {
		console.error("Echo request failed:", error);
		document.getElementById("echoOutput").style.display = "block";
		document.getElementById("echoValue").textContent = "Error sending request";
	}
}

// Initialize on page load
window.addEventListener("DOMContentLoaded", () => {
	// Fetch device info immediately and every 2 seconds
	fetchDeviceInfo();
	setInterval(fetchDeviceInfo, 2000);

	// Setup echo input handler
	const echoInput = document.getElementById("echoInput");
	echoInput.addEventListener("keypress", (event) => {
		if (event.key === "Enter") {
			sendEcho(echoInput.value);
			echoInput.value = "";
		}
	});
});

