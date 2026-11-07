// Read src/ dir; create build
// replace all references within resources with {{CDN_URL}} to be replaced with the CDN_URL env variable
const fs = require('fs');
const path = require('path');

const srcDir = path.join(__dirname, 'src');
const buildDir = path.join(__dirname, 'build');
const fileExtensionsToReplace = ['.js', '.css', '.html'];

if (fs.existsSync(buildDir)) {
    fs.rmSync(buildDir, { recursive: true, force: true });
}
fs.mkdirSync(buildDir);

// Copy all files and directories from src to build, replacing references to {{CDN_URL}} in .js, .css, and .html files

function copyAndReplaceReferences(src, dest) {
  const items = fs.readdirSync(src);

  items.forEach(item => {
    const srcPath = path.join(src, item);
    const destPath = path.join(dest, item);
    const stats = fs.statSync(srcPath);

    // Only scan within .js, .css, .html files for references to replace

    if (stats.isDirectory()) {
      if (!fs.existsSync(destPath)) {
        fs.mkdirSync(destPath);
      }
      copyAndReplaceReferences(srcPath, destPath);
    } else if (stats.isFile()) {
      // Only process .js, .css, and .html files
      if (fileExtensionsToReplace.includes(path.extname(srcPath))) {
        let content = fs.readFileSync(srcPath, 'utf8');
        content = content.replace(/{{CDN_URL}}/g, process.env.CDN_URL || '');
        fs.writeFileSync(destPath, content);
      } else {
        fs.copyFileSync(srcPath, destPath);
      }
    }
  });
}

copyAndReplaceReferences(srcDir, buildDir);

console.log('Build completed successfully and outputted to build/ directory.');