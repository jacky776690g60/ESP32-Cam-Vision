let frameRate   = 10;
let isStreaming = false;
const frameQueue   = [];
const maxQueueSize = 20;
/** canvas for drawing image */
const canvas = document.getElementById('video-canvas');
const ctx    = canvas.getContext('2d');


const frameRateInput = document.getElementById('frame_rate');


/** Fetching frames continuously in the background */
async function fetchFrames() {
  while (isStreaming) {
    try {
      const response = await fetch('/batch_stream');
      if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
      const frames = await response.json();
      
      frames.forEach(b64 => {
        if (frameQueue.length < maxQueueSize) frameQueue.push(b64);
      });
    } catch (error) {
      console.error('Stream error:', error);
    }
  }
}

/** Displaying frames from the queue */
async function displayFrames() {
  while (isStreaming) {
    if (frameQueue.length > 0) {
      const b64 = frameQueue.shift();
      await paintFrame(b64);
    } else {
      await delay(100); /** @note If no frames are available, wait a bit */
    }
    await delay(1000 / frameRate); /** throttling based on frame rate */
  }
}



/**
 * 
 * @param {string} b64 - Base64-encoded JPEG image string.
 * 
 * @returns {Promise<void>} - Resolves when the frame is painted.
 */
function paintFrame(b64) {
  return new Promise((resolve, reject) => {
    const img = new Image();
    img.onload = () => { 
      if (canvas.width !== img.width || canvas.height !== img.height) { // resized only once when loaded
        canvas.width  = img.width;
        canvas.height = img.height;
      }
      ctx.drawImage(img, 0, 0);
      resolve();
    };
    img.onerror = reject;
    img.src = `data:image/jpeg;base64,${b64}`;
  });
}



async function startStreaming() {
  if (isStreaming) return;
  isStreaming = true;
  /** @note Start fetching and displaying concurrently */
  // fetchFrames();
  displayFrames();
}

function stopStreaming() {
  isStreaming = false;
}

document.addEventListener("DOMContentLoaded", startStreaming);
window.addEventListener('beforeunload', stopStreaming);




















function delay(ms) {
  return new Promise(res => setTimeout(res, ms));
}

function debounce(func, delay) {
  let debounceTimer;
  return function(...args) {
    const context = this;
    clearTimeout(debounceTimer);
    debounceTimer = setTimeout(() => func.apply(context, args), delay);
  }
}




const frameSizeMappings = {
  'QQVGA': { width: 160,  height: 120 },
  'QCIF':  { width: 176,  height: 144 },
  'QVGA':  { width: 320,  height: 240 },
  'CIF':   { width: 400,  height: 296 },
  'HVGA':  { width: 480,  height: 320 },
  'VGA':   { width: 640,  height: 480 },
  'SVGA':  { width: 800,  height: 600 },
  'XGA':   { width: 1024, height: 768 },
  'SXGA':  { width: 1280, height: 1024 },
  'UXGA':  { width: 1600, height: 1200 }
};
const inputs = {
  frame_size:     document.getElementById("frame_size"),
  frame_rate:     document.getElementById("frame_rate"),
  jpeg_quality:   document.getElementById("jpeg_quality"),
  brightness:     document.getElementById("brightness"),
  ae_level:       document.getElementById("ae_level"),
  aec:            document.getElementById("aec"),
  aec_value:      document.getElementById("aec_value"),
  special_effect: document.getElementById("special_effect"),
  sharpness:      document.getElementById("sharpness"),
  saturation:     document.getElementById("saturation"),
  colorbar:       document.getElementById("colorbar"),
  denoise:        document.getElementById("denoise"),
  contrast:       document.getElementById("contrast"),
};

function sendSettingUpdate(field, value) {
  const data = {};
  data[field] = value;

  fetch('/settings', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json;charset=UTF-8'
    },
    body: JSON.stringify(data)
  })
  .then(response => {
    if (response.ok) {
      console.log(`Setting '${field}' updated successfully.`);
      if (field === 'frame_size') {
        const selectedSize = frameSizeMappings[value];
        console.log(`Selected Frame Size: ${selectedSize.width}x${selectedSize.height}`);
        // Update canvas or other elements if necessary
      }
    } else {
      console.error(`Failed to update '${field}'. Server responded with status ${response.status}.`);
    }
  })
  .catch(error => {
    console.error(`Error updating '${field}': ${error}`);
  });
}



document.addEventListener("DOMContentLoaded", ()=> {
  for (const [field, elem] of Object.entries(inputs)) {
    if (elem.tagName.toLowerCase() === 'input' && elem.type === 'range') {
      elem.addEventListener('input', debounce(
        function(event) {
          const value = parseInt(event.target.value, 10);
          sendSettingUpdate(field, value);
        }, 300)
      );
    } else {
      elem.addEventListener('change', function(event) {
        let value = event.target.value;
        if (event.target.type === 'number') {
          value = parseInt(value, 10);
        }
        sendSettingUpdate(field, value);
      });
    }
  }
});









/**
 * @param {HTMLInputElement} slider 
 */
function updateRangeValue(slider) {
  const label = slider.previousElementSibling;
  if (!label) return;

  const span = label.querySelector('span');
  if (!span) return;

  span.textContent = slider.value;
}
window.updateRangeValue = updateRangeValue; // must attach to window if put here