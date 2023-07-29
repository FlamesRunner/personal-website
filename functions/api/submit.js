/**
 * @api {post} /api/submit Submit
 */
export async function onRequestPost(context) {
    try {
        let input = await context.request.json();
        
        return new Response(JSON.stringify({
            "success": true,
            "message": "Your message has been sent!"
        }), {
            headers: {
                'Content-Type': 'application/json'
            }
        });
    } catch (err) {
        return new Response('Error reading response', { status: 400 });
    }
}