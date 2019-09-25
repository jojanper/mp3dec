const shelljs = require('shelljs');
const childProcess = require('child_process');

function execute(cmd) {
    return new Promise((resolve, reject) => {
        const child = childProcess.exec(cmd);

        child.stdout.on('data', function (data) {
            console.log(data.trim());
        });

        child.on('exit', function (code) {
            if (code != 0) {
                return reject(code);
            }

            resolve(code);
        });
    });
}

async function runBuild(options) {
    shelljs.rm('-rf', options.folder);
    shelljs.mkdir('-p', options.folder);

    let cmd = [
        'cmake',
        '-H.',
        `-B./${options.folder}`,
        '-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON',
        `-DCMAKE_BUILD_TYPE=${options.type}`
    ];
    await execute(cmd.join(' '));

    cmd = [
        `cd ${options.folder} &&`,
        `cmake --build . --config ${options.type}`
    ];
    await execute(cmd.join(' '));
}

async function runTests(options) {
    cmd = [
        `cd ${options.folder} &&`,
        'GTEST_COLOR=1 ctest --verbose'
    ];
    await execute(cmd.join(' '));
}

module.exports = program => {
    program
        .command('x64-build')
        .description('Create Linux/x64 build')
        .option('--folder <folder>', 'Build folder', 'build')
        .option('--type <type>', 'Build type', 'Release')
        .action(runBuild);

    program
        .command('x64-tests')
        .description('Execute Linux/x64 tests')
        .option('--folder <folder>', 'Build folder', 'build')
        .action(runTests);
};
