#!/bin/bash

# GitHub Actions build script
# Root project directory: $PROJECT_DIR

# Check if environment variables are set
ENV_VARS=("R2_ACCESS_KEY_SECRET" "R2_ACCESS_KEY_ID" "R2_API_URL" "R2_DEFAULT_REGION" "R2_BUCKET_NAME")
for VAR in ${ENV_VARS[@]}; do
  if [ -z "${!VAR}" ]; then
    echo "Error: Environment variable $VAR is not set."
    exit 1
  fi
done

echo "Generating thumbnails and WEBP images for the gallery..."

# Enter the gallery directory
#cd ./src/assets/media/gallery
cd $PROJECT_DIR/src/assets/media/gallery

# Copy the JPG files to the thumbnails directory
mkdir -p ./thumbnails
rm -rf ./thumbnails/*
cp *.jpg ./thumbnails

# Resize the JPG images by 50%
cd ./thumbnails
mogrify -resize 50% *.jpg

# Convert the JPG images to WEBP at 80% quality
mogrify -format webp -quality 80 *.jpg

# Remove the JPG images
rm *.jpg

# Return to the top-level directory
cd $PROJECT_DIR

echo "Thumbnails and WEBP images generated successfully!"

# Attempt to upload the assets via S3 API (with env. variables R2_ACCESS_KEY_SECRET, R2_ACCESS_KEY_ID, R2_API_URL, R2_DEFAULT_REGION, R2_BUCKET_NAME)
echo "Uploading the assets build to CloudFlare R2 (S3 API)..."

# Create profile
aws configure set aws_access_key_id $R2_ACCESS_KEY_ID
aws configure set aws_secret_access_key $R2_ACCESS_KEY_SECRET
aws configure set default.region $R2_DEFAULT_REGION

# Delete the existing assets
aws s3 rm s3://$R2_BUCKET_NAME/assets --recursive --endpoint-url $R2_API_URL

# Upload the new assets
aws s3 cp ./src/assets s3://$R2_BUCKET_NAME/assets --recursive --endpoint-url $R2_API_URL

echo "Assets uploaded successfully!"

echo "Build complete!"
