/*
    Create build/ dir
    Read posts from './posts' (HTML posts)
    Copy src/* to build/
    Modify build/index.html, replace <!--[POST_AREA]--> with the list of posts
    All posts' filenames formatted as dd-mm-yyyy-url-slug.html
*/

import fs from "fs";
import path from "path";

const POSTS_DIR = path.join(__dirname, "posts");
const BUILD_DIR = path.join(__dirname, "build");

type PostsParsedT = {
    [key: string]: { // filename
        date: Date; // date
        title: string; // url-slug
        filename: string; // original filename
    };
}

// Create build directory if it doesn't exist
if (!fs.existsSync(BUILD_DIR)) {
  fs.mkdirSync(BUILD_DIR);
}

// Read posts from the posts directory and parse
const posts = fs.readdirSync(POSTS_DIR).filter(file => file.endsWith(".html"));
const SRC_DIR = path.join(__dirname, "src");
const POSTS_PARSED = {} as PostsParsedT;
for (let post of posts) {
    const [day, month, year, ...titleParts] = post.replace(".html", "").split("-");
    const date = new Date(`${year}-${month}-${day}`);
    const title = titleParts.join("-");
    POSTS_PARSED[post] = { date, title, filename: post };
}

// Sort posts by date, assign ID, newest to oldest
const sortedPosts = Object.values(POSTS_PARSED).sort((a, b) => b.date.getTime() - a.date.getTime());

// Generate <post id="postId">POST_HTML</post> for each post and insert in order
let postsHTML = "";

for (let id = 0; id < sortedPosts.length; id++) {
    let post = sortedPosts[id];
    const postContent = fs.readFileSync(path.join(POSTS_DIR, post!.filename), "utf-8");
    postsHTML += `<post id="${id}">${postContent}</post>\n`;
}

// Copy all files from src to build
const srcFiles = fs.readdirSync(SRC_DIR);
for (let file of srcFiles) {
    const srcPath = path.join(SRC_DIR, file);
    const destPath = path.join(BUILD_DIR, file);
    fs.copyFileSync(srcPath, destPath);
}

// Read index.html, replace <!--[POST_AREA]--> with postsHTML, write to build/index.html
const indexHTML = fs.readFileSync(path.join(SRC_DIR, "index.html"), "utf-8");
const modifiedIndexHTML = indexHTML.replace("<!--[POST_AREA]-->", postsHTML);
fs.writeFileSync(path.join(BUILD_DIR, "index.html"), modifiedIndexHTML);

console.log("Build completed successfully.");