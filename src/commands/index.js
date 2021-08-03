/**
 * Management commands.
 */
const path = require('path');
const program = require('commander');
const fs = require('fs');

const POSTFIX = 'commands';

// Read all modules and initialize commander
fs.readdirSync(path.join(__dirname, POSTFIX))
    .forEach(file => {
        // eslint-disable-next-line global-require, import/no-dynamic-require
        require(`./${POSTFIX}/${file}`)(program);
    });

program.on('command:*', () => {
    console.error('Invalid command: %s\nSee --help for a list of available commands.', program.args.join(' '));
    process.exit(1);
});

program.parse(process.argv);
