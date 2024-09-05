import { initializeApp } from 'firebase/app';
import { getDatabase, ref, onValue } from 'firebase/database';
import Chart from 'chart.js/auto'; // Import Chart.js

const firebaseConfig = {
  apiKey: "AIzaSyAS_rVBCn2vIKNH7Vz4vriES78rrySmFh8",
  authDomain: "ccus-detection.firebaseapp.com",
  databaseURL: "https://ccus-detection-default-rtdb.firebaseio.com",
  projectId: "ccus-detection",
  storageBucket: "ccus-detection.appspot.com",
  messagingSenderId: "5207527502",
  appId: "1:5207527502:web:a0b66837060adebb69731f",
  measurementId: "G-CYL6P0JWZJ"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

// References to CO2 and temperature in the database
const co2Ref = ref(db, '/co2');
const tempRef = ref(db, '/temperature');

// Function to update the chart with new data
const co2Data = [];
const timeLabels = [];
const ctx = document.getElementById('co2Chart').getContext('2d');
const chart = new Chart(ctx, {
  type: 'line',
  data: {
    labels: timeLabels,
    datasets: [{
      label: 'CO2 Levels (ppm)',
      backgroundColor: 'rgba(255, 0, 0, 0.5)',
      borderColor: 'rgba(255, 0, 0, 1)',
      data: co2Data,
      fill: false,
    }]
  },
  options: {
    responsive: true,
    maintainAspectRatio: false,
    scales: {
      x: { display: true, title: { display: true, text: 'Time', color: '#e0e0e0' } },
      y: { display: true, title: { display: true, text: 'CO2 Levels (ppm)', color: '#e0e0e0' } }
    },
    plugins: { legend: { labels: { color: '#e0e0e0' } } }
  }
});

function updateChart(co2, time) {
  if (co2Data.length > 20) {
    co2Data.shift();
    timeLabels.shift();
  }
  co2Data.push(co2);
  timeLabels.push(time);
  chart.update(); // Refresh the chart
}

// Update the page when CO2 data changes
onValue(co2Ref, (snapshot) => {
  const co2 = snapshot.val();
  document.getElementById('co2').innerHTML = 'CO2: ' + co2 + ' ppm';
  updateChart(co2, new Date().toLocaleTimeString());
}, (error) => {
  console.error("Error fetching CO2 data:", error);
});

// Update the page when temperature data changes
onValue(tempRef, (snapshot) => {
  const temp = snapshot.val();
  document.getElementById('temp').innerHTML = 'Temp: ' + temp + ' °C';
}, (error) => {
  console.error("Error fetching temperature data:", error);
});
