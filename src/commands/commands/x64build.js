const util = require('util');
const fs = require('fs');
const childProcess = require('child_process');

function execute(cmd) {
    return new Promise((resolve, reject) => {
        const child = childProcess.spawn(cmd, {
            shell: true,
            stdio: 'inherit'
        });

        child.on('exit', code => {
            if (code !== 0) {
                console.log(code);
                return reject(code);
            }

            resolve(code);
        });
    });
}

async function runBuild(options) {
    let promiseFn;

    promiseFn = util.promisify(fs.rmdir);
    await promiseFn(options.folder, { recursive: true });
    promiseFn = util.promisify(fs.mkdir);
    await promiseFn(options.folder);

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
        .action(options => runBuild(options).catch(err => {
            console.log(err); process.exit(1);
        }));

    program
        .command('x64-tests')
        .description('Execute Linux/x64 tests')
        .option('--folder <folder>', 'Build folder', 'build')
        .option('--memcheck', 'Enable memory checker', false)
        .action(options => runTests(options).catch(err => {
            console.log(err); process.exit(1);
        }));

    program
        .command('x64-coverage')
        .description('Build code coverage report')
        .option('--folder <folder>', 'Build folder', 'build')
        .action(options => runCoverage(options).catch(err => {
            console.log(err); process.exit(1);
        }));
};
