import nodemailer from 'nodemailer';

const sendResponse = (obj) => {
    return new Response(JSON.stringify(obj), {
        headers: {
            'Content-Type': 'application/json'
        }
    });
}

const createMailClient = () => {
    return nodemailer.createTransport({
      host: "smtp-pulse.com",
      port: 465,
      secure: true,
      auth: {
        user: "andrew@andrew-hong.me",
        pass: "meqmav-hyqxem-nypbA0",
      }
    });
  }

/**
 * @api {post} /api/submit Submit
 */
export async function onRequestPost(context) {
    const token = body.get('cf-turnstile-response');
	const ip = request.headers.get('CF-Connecting-IP');

    try {
        let input = await context.request.json();
        let { name, email, message } = input;
        let token = input['cf-turnstile-response'];
        if (!name || !email || !message) {
            return sendResponse({
                "success": false,
                "message": "Please fill out all fields."
            });
        }

        const url = 'https://challenges.cloudflare.com/turnstile/v0/siteverify';
        const formData = new FormData();
        formData.append('cf-turnstile-response', token);
        formData.append('ip', ip);
        formData.append('secret', '0x4AAAAAAAIAaJowwcIt0oSvMtLqszseMLA');
        const response = await fetch(url, {
            method: 'POST',
            body: formData
        });
        const result = await response.json();

        if (result.success !== true) {
            return sendResponse({
                "success": false,
                "message": "Your request is missing the CloudFlare Turnstile token. Please refresh the page and try again."
            });
        }

        // Send email
        const mailClient = createMailClient();
        await mailClient.sendMail({
            from: 'noreply@ahong.ca',
            to: 'me@ahong.ca',
            subject: 'New message from ' + name,
            text: email
        });

        return sendResponse({
            "success": true,
            "message": "Message sent successfully."
        });
    } catch (err) {
        return new Response('Error reading response', { status: 400 });
    }
}