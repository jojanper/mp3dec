const mp3Dec = require('./build/Release/mp3dec.node');

console.log('mp3Dec', mp3Dec);

const classInstance = new mp3Dec.ClassExample(4.3);
console.log('Testing class initial value : ',classInstance.getValue());
console.log('After adding 3.3 : ',classInstance.add(3.3));

module.exports = mp3Dec;
