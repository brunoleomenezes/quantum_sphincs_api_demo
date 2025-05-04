# src/api/auth.py

import os
from fastapi import Request, HTTPException
from fastapi.security import HTTPBearer
from supabase import create_client, Client

SUPABASE_URL = os.getenv("SUPABASE_URL")
SUPABASE_KEY = os.getenv("SUPABASE_API_KEY")

supabase: Client = create_client(SUPABASE_URL, SUPABASE_KEY)
auth_scheme = HTTPBearer()

async def verify_token(request: Request):
    token = await auth_scheme(request)
    try:
        user = supabase.auth.get_user(token.credentials)
        if user.user is None:
            raise HTTPException(status_code=403, detail="Invalid token.")
        return user.user
    except Exception:
        raise HTTPException(status_code=403, detail="Invalid token or expired.")
