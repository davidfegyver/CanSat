const output = document.getElementById('output');

async function control(action) {
    output.textContent = 'Sending request...';

    const response = await fetch(`/action/${action}`);

    const data = await response.text();
    output.innerHTML = data.replaceAll('\n', '<br>');
}

async function showImage(path) {
    output.textContent = 'Loading image...';
    const response = await fetch(path);

    if (!response.headers.get('content-type').startsWith('image')) {
        const data = await response.text();
        output.textContent = data;
        return;
    }

    const data = await response.blob();
    const url = URL.createObjectURL(data);
    output.innerHTML = `<img src="${url}" width="200">`;
}