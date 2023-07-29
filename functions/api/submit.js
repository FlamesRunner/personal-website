const sendgridApiKey = 'SG.VgJGSPdPQ3m5qfKgK-GoHw.z8Jr6KgQG5kQ3VVldzKJ8YGvHBIpU5ucS3GccK7HX4c';

const sendResponse = (obj) => {
    return new Response(JSON.stringify(obj), {
        headers: {
            'Content-Type': 'application/json'
        }
    });
}

const sendEmail = async (to, from, subject, text) => {
    const url = 'https://api.sendgrid.com/v3/mail/send';
    return fetch(url, {
        method: 'POST',
        headers: {
            'Authorization': `Bearer ${sendgridApiKey}`,
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            "personalizations": [
                {
                    "to": [
                        {
                            "email": to
                        }
                    ],
                    "subject": subject
                }
            ],
            "from": {
                "email": from
            },
            "content": [
                {
                    "type": "text/plain",
                    "value": text
                }
            ]
        })
    });
}

/**
 * @api {post} /api/submit Submit
 */
export async function onRequestPost(context) {
    try {
        let ip = context.request.headers.get('CF-Connecting-IP');
        let input = await context.request.json();
        let { name, email, message } = input;
        if (!name || !email || !message) {
            return sendResponse({
                "success": false,
                "message": "Please fill out all fields."
            });
        }

        if (!input.hasOwnProperty('cf-turnstile-response')) {
            return sendResponse({
                "success": false,
                "message": "Your request is missing the CloudFlare Turnstile token. Please refresh the page and try again."
            });
        }

        const url = 'https://challenges.cloudflare.com/turnstile/v0/siteverify';
        const formData = new FormData();
        formData.append('response', input['cf-turnstile-response']);
        formData.append('remoteip', ip);
        formData.append('secret', '0x4AAAAAAAIAaJowwcIt0oSvMtLqszseMLA');
        const response = await fetch(url, {
            method: 'POST',
            body: formData
        });
        const result = await response.json();

        if (!result.success) {
            return sendResponse({
                "success": false,
                'token': input['cf-turnstile-response'],
                'ip': ip,
                "message": "Your request was invalid. Please try again."
            });
        }

        // Send email
        const sendgridResponse = await sendEmail('andrew@ahong.ca', 'noreply@ahong.ca', `New message from ahong.ca (${name}, ${email})`, message);
        if (sendgridResponse.status !== 202) {
            return sendResponse({
                "success": false,
                "message": "Error sending message."
            });
        }

        return sendResponse({
            "success": true,
            "message": "Message sent successfully."
        });
    } catch (err) {
        console.error(err);
        return new Response('Error reading response', { status: 400 });
    }
}