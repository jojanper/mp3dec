const shelljs = require('shelljs');
const childProcess = require('child_process');

function execute(cmd) {
    return new Promise((resolve, reject) => {
        const child = childProcess.exec(cmd);

        // Live console output
        child.stdout.on('data', data => console.log(data.trim()));

        child.on('exit', code => {
            if (code !== 0) {
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
    const cmd = [
        `cd ${options.folder} &&`
    ];

    if (options.memcheck) {
        cmd.push('GTEST_COLOR=1 ctest -V -T memcheck && cat Testing/Temporary/MemoryChecker.*.log');
    } else {
        cmd.push('GTEST_COLOR=1 ctest --verbose');
    }

    await execute(cmd.join(' '));
}

async function runCoverage(options) {
    const cmd = [
        `cd ${options.folder} &&`,
        'make coverage'
    ];
    await execute(cmd.join(' '));
}

module.exports = program => {
    program
        .command('x64-build')
        .description('Create Linux/x64 build')
        .option('--folder <folder>', 'Build folder', 'build')
        .option('--type <type>', 'Build type', 'Release')
        .action(options => runBuild(options).catch(err => { throw new Error(err); }));

    program
        .command('x64-tests')
        .description('Execute Linux/x64 tests')
        .option('--folder <folder>', 'Build folder', 'build')
        .option('--memcheck', 'Enable memory checker', false)
        .action(options => runTests(options).catch(err => { throw new Error(err); }));

    program
        .command('x64-coverage')
        .description('Build code coverage report')
        .option('--folder <folder>', 'Build folder', 'build')
        .action(options => runCoverage(options).catch(err => { throw new Error(err); }));
};
