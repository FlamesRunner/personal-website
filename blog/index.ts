import express from "express";
import fs from "fs";

const app = express();

const DEFAULT_OR_BUILD_DIR = fs.existsSync("build") ? "build" : "src";

app.use(express.static(DEFAULT_OR_BUILD_DIR));

app.listen(3000, () => {
  console.log("Static server started on port http://localhost:3000");
});