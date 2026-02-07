# Use the latest Alpine image as the base
FROM alpine:latest AS build

# Install Node.js and npm
RUN apk add --no-cache nodejs npm

# Set the working directory
WORKDIR /app

# Copy package.json and package-lock.json
COPY package*.json ./

# Install dependencies
RUN npm install

# Copy the rest of the application files
COPY . .

# Build the application
RUN npm run build

# Use NGINX as the base image for serving the app
FROM nginx:alpine

# Copy the build output to the NGINX html directory
COPY --from=build /app/build /usr/share/nginx/html

# Remove server_tokens
RUN sed -i 's/http {/http {\n    server_tokens off;/' /etc/nginx/nginx.conf

# Expose port 80
EXPOSE 80

# Start NGINX
CMD ["nginx", "-g", "daemon off;"]
