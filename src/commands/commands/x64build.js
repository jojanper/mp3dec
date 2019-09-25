/*
const fs = require('fs');
const path = require('path');
const util = require('util');
const shelljs = require('shelljs');
const childProcess = require('child_process');
const rimraf = require("rimraf");
*/

function runBuild(options) {
    console.log(options);

    /*
    const promise = util.promisify(childProcess.exec);

    const manifestCache = manifestToCachePath(cacheprefix);
    const cachePath = path.join(sourceFolder, manifestCache);
    createCacheFolder(cachePath);

    const zipFile = `${manifestCache}/${output}`;

    // Create zip command
    let cmd = util.format('cd %s && cmake -E tar cfv %s --format=zip -- %s', sourceFolder, zipFile, inputs.join(' '));
    console.log('Zip command: ', cmd);

    // Execute the zip command, wait for the response
    let response = await promiseExec(promise(cmd));
    if (response[0]) {
        const msg = util.format('Failed to create upload package %s:\n', output, response[0]);
        utils.misc.logError(msg);
    }
    */
}

module.exports = program => {
    program
        .command('x64-build')
        .description('Create Linux/x64 build')
        .option('--folder <folder>', 'Build folder', 'build')
        .option('--type <type>', 'Build type', 'Release')
        .action(runBuild);
};
