/*
 * HTTP Server Sample - Simple JavaScript
 * Updates device uptime every second
 *
 * SPDX-License-Identifier: Apache-2.0
 */

async function fetchUptime()
{
	try {
		const response = await fetch("/uptime");
		if (!response.ok) {
			throw new Error(`HTTP error! status: ${response.status}`);
		}

		const uptime = await response.text();
		const uptimeElement = document.getElementById("uptime");
		if (uptimeElement) {
			uptimeElement.innerHTML = "Uptime: " + uptime + " milliseconds";
		}
	} catch (error) {
		console.error("Failed to fetch uptime:", error);
	}
}

window.addEventListener("DOMContentLoaded", () => {
	/* Fetch uptime once per second */
	setInterval(fetchUptime, 1000);
	/* Fetch immediately on page load */
	fetchUptime();
});
